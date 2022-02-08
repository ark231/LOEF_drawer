#include "loef_drawer.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QInputDialog>
#include <QJsonArray>
#include <QMouseEvent>
#include <QScreen>
#include <QtGlobal>
#include <algorithm>
#include <iterator>

#include "LOEF_QPainter.hpp"
#include "boost/numeric/odeint.hpp"
#include "debug_outputs.hpp"
#include "experimental/differential_equation.hpp"
#include "experimental/electric_potential.hpp"
#include "experimental/lazy_helper.hpp"
#include "general_consts.hpp"
#include "units.hpp"
namespace LOEF {
class state_charge_selected_ {
    std::vector<std::pair<id_type, vec2d>> selected_ids_and_offsets_;
    LOEF_drawer *parent_drawer_ = nullptr;

   public:
    state_charge_selected_(LOEF_drawer *parent);
    operator bool();
    void set_selected(id_type idx, vec2d offset = vec2d());
    void unselect(id_type id);
    void unselect_all();
    std::vector<LOEF::id_type> get_selected();
    vec2d get_offset(id_type);
    void update_offset(vec2d);
    bool is_selected(id_type);
};
state_charge_selected_::state_charge_selected_(LOEF_drawer *parent) : parent_drawer_(parent) {}
state_charge_selected_::operator bool() { return !selected_ids_and_offsets_.empty(); }
void state_charge_selected_::set_selected(LOEF::id_type id, vec2d offset) {
    selected_ids_and_offsets_.push_back({id, offset});
}
void state_charge_selected_::unselect(id_type id) {
    auto itr_to_erase = std::find_if(selected_ids_and_offsets_.begin(), selected_ids_and_offsets_.end(),
                                     [id](std::pair<id_type, vec2d> element) { return element.first == id; });
    selected_ids_and_offsets_.erase(itr_to_erase);
}
void state_charge_selected_::unselect_all() { selected_ids_and_offsets_.clear(); }
std::vector<LOEF::id_type> state_charge_selected_::get_selected() {
    std::vector<LOEF::id_type> result;
    std::transform(selected_ids_and_offsets_.begin(), selected_ids_and_offsets_.end(), std::back_inserter(result),
                   [](std::pair<id_type, vec2d> element) { return element.first; });
    return result;
}
vec2d state_charge_selected_::get_offset(id_type id) {
    Q_ASSERT(is_selected(id));
    auto result_pair = std::find_if(selected_ids_and_offsets_.begin(), selected_ids_and_offsets_.end(),
                                    [id](std::pair<id_type, vec2d> element) { return element.first == id; });
    return result_pair->second;
}
void state_charge_selected_::update_offset(vec2d mouse_pos) {
    for (auto &selected_id_and_offset : selected_ids_and_offsets_) {
        auto &tmp = selected_id_and_offset;
        auto charge_info = parent_drawer_->get_fixed_charge_info(tmp.first);
        vec2d selected_pos(std::get<1>(charge_info), std::get<2>(charge_info));
        tmp = {tmp.first, selected_pos - mouse_pos};
    }
}
bool state_charge_selected_::is_selected(id_type id) {
    return std::any_of(selected_ids_and_offsets_.begin(), selected_ids_and_offsets_.end(),
                       [id](std::pair<id_type, vec2d> element) { return element.first == id; });
}

class id_handler {
    LOEF::id_type next_id;

   public:
    id_handler(LOEF::id_type initial_id = 0);
    LOEF::id_type new_id();
};
id_handler::id_handler(LOEF::id_type initial_id) : next_id(initial_id) {}
LOEF::id_type id_handler::new_id() {
    return next_id++;  //足す前の値を返す
}
}  // namespace LOEF

LOEF_drawer::LOEF_drawer(QWidget *parent) : QWidget(parent) {
    double dpi = QGuiApplication::screens().at(0)->physicalDotsPerInch();
    this->dpmm_ = (dpi / 25.4) * LOEF::dot_per_millimetre;
    charge_selected_manually_ = new LOEF::state_charge_selected_(this);
    charge_selected_automatically_ = new LOEF::state_charge_selected_(this);
    fixed_charge_id_handler_ = new LOEF::id_handler;
    charge_pen_id_handler_ = new LOEF::id_handler;
    this->setFocusPolicy(Qt::StrongFocus);
}
void LOEF_drawer::paintEvent(QPaintEvent *) {
    LOEF::painter painter(this);
    painter.set_resolution(dpmm_);
    int width = this->width();
    int height = this->height();
    for (LOEF::millimetre_quantity x = 0.0 * LOEF::millimetre; x < static_cast<double>(width) / dpmm_;
         x += 1.0 * LOEF::millimetre) {
        switch (static_cast<int>(x.value()) % 10) {
            case 0:
                painter.drawLine(x * dpmm_, 0, x * dpmm_, 3.0 * LOEF::millimetre * dpmm_);
                break;
            case 5:
                painter.drawLine(x * dpmm_, 0, x * dpmm_, 2.0 * LOEF::millimetre * dpmm_);
                break;
            default:
                painter.drawLine(x * dpmm_, 0, x * dpmm_, 1.0 * LOEF::millimetre * dpmm_);
                break;
        }
    }
    for (LOEF::millimetre_quantity y = 0.0 * LOEF::millimetre; y < static_cast<double>(width) / dpmm_;
         y += 1.0 * LOEF::millimetre) {
        switch (static_cast<int>(y.value()) % 10) {
            case 0:
                painter.drawLine(0, y * dpmm_, 3.0 * LOEF::millimetre * dpmm_, y * dpmm_);
                break;
            case 5:
                painter.drawLine(0, y * dpmm_, 2.0 * LOEF::millimetre * dpmm_, y * dpmm_);
                break;
            default:
                painter.drawLine(0, y * dpmm_, 1.0 * LOEF::millimetre * dpmm_, y * dpmm_);
                break;
        }
    }
    for (const auto &charge : fixed_charges_) {
        painter.draw_fixed_charge(charge.second, charge_selected_manually_->is_selected(charge.first) ||
                                                     charge_selected_automatically_->is_selected(charge.first));
    }
    if (!draw_LOEF_requested) {
        return;
    }
    // lazy
    if (this->electric_potential_handler->color_enabled) {
        painter.drawImage(0, 0, prepare_electric_potential_image());
    }
    if (this->electric_potential_handler->draw_sample_line) {
        std::vector<LOEF::vec2d> line_ends;
        for (const auto &charge : fixed_charges_) {
            if (charge.second.quantity().value() == 0.0) {
                line_ends.push_back(charge.second.position());
            }
        }
        if (line_ends.size() == 3) {
            LOEF::vec2d start;
            LOEF::vec2d end;
            auto diff_01 = (line_ends[0] - line_ends[1]).length().value();
            auto diff_02 = (line_ends[0] - line_ends[2]).length().value();
            auto diff_12 = (line_ends[1] - line_ends[2]).length().value();
            auto min_diff = std::min({diff_01, diff_02, diff_12});
            if (min_diff == diff_01) {
                start = line_ends[2];
                end = (line_ends[0] + line_ends[1]) / 2.0;
            } else if (min_diff == diff_02) {
                start = line_ends[1];
                end = (line_ends[0] + line_ends[2]) / 2.0;
            } else if (min_diff == diff_12) {
                start = line_ends[0];
                end = (line_ends[1] + line_ends[2]) / 2.0;
            }
            painter.save();
            QPen pen;
            pen.setColor(Qt::green);
            painter.setPen(pen);
            painter.drawLine(start.to_QPointF(dpmm_), end.to_QPointF(dpmm_));
            painter.restore();
        }
    } else if (this->electric_potential_handler->draw_sample_rectangle) {
        std::vector<LOEF::vec2d> line_ends;
        for (const auto &charge : fixed_charges_) {
            if (charge.second.quantity().value() == 0.0) {
                line_ends.push_back(charge.second.position());
            }
        }
        auto start_x = boost::units::fmin(line_ends[0].x(), line_ends[1].x()) * dpmm_;
        auto start_y = boost::units::fmin(line_ends[0].y(), line_ends[1].y()) * dpmm_;
        auto end_x = boost::units::fmax(line_ends[0].x(), line_ends[1].x()) * dpmm_;
        auto end_y = boost::units::fmax(line_ends[0].y(), line_ends[1].y()) * dpmm_;
        if (line_ends.size() == 2) {
            painter.save();
            QPen pen;
            pen.setColor(Qt::green);
            painter.setPen(pen);
            painter.drawRect(start_x, start_y, end_x - start_x, end_y - start_y);
            painter.restore();
        }
    }
    if (this->electric_potential_handler->disable_LOEF == true) {
        return;
    }
    // end lazy
    LOEF::clear_pens_arrival_to_fixed_charges(fixed_charges_.begin(), fixed_charges_.end());
    decltype(fixed_charges_) positive_fixed_charges;
    decltype(fixed_charges_) newtral_fixed_charges;
    decltype(fixed_charges_) nonpositive_fixed_charges;
    decltype(fixed_charges_) negative_fixed_charges;
    std::partition_copy(fixed_charges_.begin(), fixed_charges_.end(),
                        std::inserter(positive_fixed_charges, positive_fixed_charges.end()),
                        std::inserter(nonpositive_fixed_charges, nonpositive_fixed_charges.end()),
                        [](decltype(*(fixed_charges_.begin())) &charge) {
                            return charge.second.quantity() > 0.0 * LOEF::boostunits::coulomb;
                        });
    std::partition_copy(nonpositive_fixed_charges.begin(), nonpositive_fixed_charges.end(),
                        std::inserter(newtral_fixed_charges, newtral_fixed_charges.end()),
                        std::inserter(negative_fixed_charges, negative_fixed_charges.end()),
                        [](decltype(*(fixed_charges_.begin())) &charge) {
                            return charge.second.quantity() == 0.0 * LOEF::boostunits::coulomb;
                        });
    calc_LOEF_from_fixed_charges(positive_fixed_charges, width, height);
    calc_LOEF_from_fixed_charges(negative_fixed_charges, width, height);
    for (auto charge_path = charge_paths_.begin(); charge_path != charge_paths_.end(); charge_path++) {
        painter.draw_LOEF_path(*(charge_path->second));
    }
}
void LOEF_drawer::calc_LOEF_from_fixed_charges(decltype(fixed_charges_) &fixed_charges, int width, int height) {
    for (const auto &charge : fixed_charges) {
        for (const auto &position :
             charge.second.calc_pen_init_pos(fixed_charges_.begin(), fixed_charges_.end(),
                                             std::abs(charge.second.quantity() * inverse_permittivity_), 360)) {
            auto pen_id = charge_pen_id_handler_->new_id();
            charge_pens_[pen_id] = LOEF::charge_pen(charge.second.quantity() > 0.0 * LOEF::boostunits::coulomb,
                                                    position, LOEF::interval_steps, width, height, dpmm_);
            charge_pens_[pen_id].origin = charge.first;
            charge_paths_[pen_id] = charge_pens_[pen_id].get_path();
        }
        prepare_LOEF_pathes();
    }
}
void LOEF_drawer::prepare_LOEF_pathes() {
    if (*(this->is_ready_made_requested)) {
        for (auto pen_itr = charge_pens_.begin(); pen_itr != charge_pens_.end(); pen_itr++) {
            auto pen = pen_itr->second;
            auto path = pen.get_path();
            /*
            if (not path->is_positive()) {
                return;
            }
            */
            using fixedMapIter = decltype(fixed_charges_.begin());
            using LOEF::experimental::mm;
            LOEF::experimental::LOEF_system<fixedMapIter>::state_type state0 = {pen.position().x().value(),
                                                                                pen.position().y().value()};
            LOEF::experimental::LOEF_system<fixedMapIter> system(fixed_charges_.begin(), fixed_charges_.end(),
                                                                 path->is_positive());
            path->moveTo(LOEF::vec2d(state0).to_QPointF(dpmm_));
            try {
                auto observer = [&, this](const LOEF::experimental::LOEF_system<fixedMapIter>::state_type &state,
                                          const double) {
                    if (LOEF::experimental::calc_shortest_distance(fixed_charges_.begin(), fixed_charges_.end(), state,
                                                                   {pen.origin}, [](LOEF::fixed_charge charge) {
                                                                       return charge.quantity() ==
                                                                              0.0 * LOEF::boostunits::coulomb;
                                                                   }) < LOEF::radius::FIXED) {
                        auto idx_entered_charge = LOEF::experimental::find_closest(
                            fixed_charges_.begin(), fixed_charges_.end(), state, [&](LOEF::fixed_charge charge) {
                                if (path->is_positive()) {
                                    return charge.quantity() >= 0.0 * LOEF::boostunits::coulomb;
                                } else {
                                    return charge.quantity() <= 0.0 * LOEF::boostunits::coulomb;
                                }
                            });
                        auto &entered_charge = fixed_charges_[idx_entered_charge];
                        auto charge_to_pos = LOEF::vec2d(state) - entered_charge.position();
                        entered_charge.pen_arrive(charge_to_pos);
                        throw std::runtime_error("entered into a charge");
                    }
                    if (not boost::units::isfinite(LOEF::vec2d(state).x()) or
                        not boost::units::isfinite(LOEF::vec2d(state).y())) {
                        throw std::runtime_error("illigal calculation happened");
                    }
                    if (not pen.is_on_screen(dpmm_)) {
                        throw std::runtime_error("go out of screen");
                    }
                    path->lineTo(LOEF::vec2d(state).to_QPointF(dpmm_));
                };
                using StepperBase = boost::numeric::odeint::runge_kutta_dopri5<
                    LOEF::experimental::LOEF_system<fixedMapIter>::state_type>;
                auto stepper = boost::numeric::odeint::make_dense_output(0.001, 0.001, StepperBase());
                boost::numeric::odeint::integrate_const(
                    stepper, std::ref(system), state0, LOEF::experimental::integrate::start_time,
                    LOEF::experimental::integrate::end_time, LOEF::experimental::integrate::dt, observer);
                /*
                boost::numeric::odeint::integrate(std::ref(system), state0, LOEF::experimental::integrate::start_time,
                                                  LOEF::experimental::integrate::end_time,
                                                  LOEF::experimental::integrate::dt, observer);
                                                  */
            } catch (std::runtime_error err) {  //握りつぶす
                qDebug() << pen_itr->first << err.what();
            }
        }
        return;
    }

    std::vector<LOEF::id_type> ids_to_erase;
    while (!charge_pens_.empty()) {
        for (auto charge_pen = charge_pens_.begin(); charge_pen != charge_pens_.end(); charge_pen++) {
            switch (charge_pen->second.step_forward(fixed_charges_.begin(), fixed_charges_.end(), dpmm_)) {
                case LOEF::step_status::FINISH:
                    ids_to_erase.push_back(charge_pen->first);
                    break;
                case LOEF::step_status::ABORT:
                    qDebug() << "@LOEF_drawer prepare_LOEF_pathes() charge pen aborted. id:" << charge_pen->first;
                    ids_to_erase.push_back(charge_pen->first);
                    break;
                case LOEF::step_status::CONTINUE:
                    break;
            }
        }
        for (const auto &id_to_erase : ids_to_erase) {
            auto iterator_to_erase = charge_pens_.find(id_to_erase);
            if (iterator_to_erase != charge_pens_.end()) {
                charge_pens_.erase(iterator_to_erase);
            }
        }
        ids_to_erase.clear();
    }
}
void LOEF_drawer::mousePressEvent(QMouseEvent *ev) {
    LOEF::vec2d pos_mouse(ev->pos(), dpmm_);
    if (this->is_multi_selecting) {
        charge_selected_manually_->update_offset(pos_mouse);
    }
    for (auto charge = fixed_charges_.begin(); charge != fixed_charges_.end(); charge++) {
        LOEF::vec2d offset = charge->second.position() - pos_mouse;
        if (offset.length() * dpmm_ <= LOEF::radius::FIXED * dpmm_) {
            if (this->is_multi_selecting && charge_selected_manually_->is_selected(charge->first)) {
                charge_selected_manually_->unselect(charge->first);
            } else {
                charge_selected_manually_->set_selected(charge->first, offset);
                charge_selected_automatically_->unselect_all();
                charge_selected_automatically_->set_selected(charge->first, offset);
                emit fixed_charge_selected(charge->first);
            }
        }
    }
}
void LOEF_drawer::mouseMoveEvent(QMouseEvent *ev) {
    LOEF::vec2d pos_mouse(ev->pos(), dpmm_);
    if (*charge_selected_manually_) {
        auto id_selecteds = charge_selected_manually_->get_selected();
        for (const auto &id_selected : id_selecteds) {
            auto new_pos = pos_mouse + charge_selected_manually_->get_offset(id_selected);
            replace_fixed_charge(id_selected, std::nullopt, new_pos);
            emit fixed_charge_position_changed(id_selected, new_pos.x(), new_pos.y());
        }
    }
}
void LOEF_drawer::mouseReleaseEvent(QMouseEvent *) {
    if (!this->is_multi_selecting) {
        charge_selected_manually_->unselect_all();
        clear_and_redraw();
    }
}
void LOEF_drawer::slot_fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity X,
                                                     LOEF::millimetre_quantity Y) {
    replace_fixed_charge(id, std::nullopt, X, Y);
}
void LOEF_drawer::slot_fixed_charge_charge_changed(LOEF::id_type id, LOEF::coulomb_quantity new_charge) {
    replace_fixed_charge(id, new_charge, std::nullopt);
}
void LOEF_drawer::replace_fixed_charge(const LOEF::id_type id,
                                       const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                                       const std::optional<LOEF::vec2d> &maybe_new_pos) {
    auto old_charge = fixed_charges_[id];
    Q_ASSERT_X((maybe_new_charge || maybe_new_pos), "LOEF_drawer::replace_fixed_charge", "no change given!");
    LOEF::coulomb_quantity new_charge;
    LOEF::vec2d new_pos;
    if (maybe_new_charge) {
        new_charge = maybe_new_charge.value();
    } else {
        new_charge = old_charge.quantity();
    }
    if (maybe_new_pos) {
        new_pos = maybe_new_pos.value();
    } else {
        new_pos = old_charge.position();
    }
    fixed_charges_[id] = LOEF::fixed_charge(new_charge, new_pos);
    clear_and_redraw();
}
void LOEF_drawer::clear_and_redraw() {
    charge_pens_.clear();
    charge_paths_.clear();
    // lazy
    if (!this->electric_potential_handler->color_use_input) {
        this->electric_potential_handler->set_current_max_abs_positive(0.0 * LOEF::experimental::V);
        this->electric_potential_handler->set_current_max_abs_negative(0.0 * LOEF::experimental::V);
        for (const auto &fixed_charge : this->fixed_charges_) {
            this->electric_potential_handler->hint_add_charge(fixed_charge.second.quantity());
        }
    }
    // end lazy
    update();
}
void LOEF_drawer::replace_fixed_charge(const LOEF::id_type id,
                                       const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                                       const LOEF::millimetre_quantity new_pos_x,
                                       const LOEF::millimetre_quantity new_pos_y) {
    replace_fixed_charge(id, maybe_new_charge, LOEF::vec2d(new_pos_x, new_pos_y));
}
void LOEF_drawer::slot_fixed_charge_destroyed(const LOEF::id_type id) {
    fixed_charges_.erase(id);
    clear_and_redraw();
}
LOEF::id_type LOEF_drawer::create_fixed_charge(LOEF::coulomb_quantity initial_fixed_charge,
                                               LOEF::millimetre_quantity initial_X,
                                               LOEF::millimetre_quantity initial_Y) {
    auto new_id = fixed_charge_id_handler_->new_id();
    fixed_charges_[new_id] = LOEF::fixed_charge(initial_fixed_charge, initial_X, initial_Y);
    clear_and_redraw();
    return new_id;
}
std::tuple<LOEF::coulomb_quantity, LOEF::millimetre_quantity, LOEF::millimetre_quantity>
LOEF_drawer::get_fixed_charge_info(LOEF::id_type id) {
    auto found_charge = fixed_charges_[id];
    return std::make_tuple(found_charge.quantity(), found_charge.position().x(), found_charge.position().y());
}
void LOEF_drawer::slot_inverse_permittivity_changed(double new_value) {
    this->inverse_permittivity_ = new_value / LOEF::boostunits::coulomb;
    for (auto &fixed_charge : fixed_charges_) {
        fixed_charge.second.update_inverse_permittivity(inverse_permittivity_);
    }
    clear_and_redraw();
}
void LOEF_drawer::request_draw_LOEF(bool yes_or_no) {
    this->draw_LOEF_requested = yes_or_no;
    clear_and_redraw();
}
QJsonObject LOEF_drawer::create_save_data() {
    QJsonObject save_data;
    save_data["inverse permittivity"] = this->inverse_permittivity_.value();
    QJsonArray json_fixed_charges;
    for (const auto &fixed_charge : fixed_charges_) {
        QJsonObject json_fixed_charge;
        json_fixed_charge["charge quantity"] = fixed_charge.second.quantity().value();
        QJsonObject json_position;
        json_position["x"] = fixed_charge.second.position().x().value();
        json_position["y"] = fixed_charge.second.position().y().value();
        json_fixed_charge["position"] = json_position;
        json_fixed_charges.append(json_fixed_charge);
    }
    save_data["fixed_charges"] = json_fixed_charges;
    return save_data;
}
void LOEF_drawer::select_fixed_charge(LOEF::id_type id) { this->charge_selected_automatically_->set_selected(id); }
void LOEF_drawer::unselect_fixed_charge(LOEF::id_type id) { this->charge_selected_automatically_->unselect(id); }
void LOEF_drawer::unselect_all_selected_fixed_charge() { this->charge_selected_automatically_->unselect_all(); }
void LOEF_drawer::destroy_all_fixed_charges() {
    fixed_charges_.clear();
    this->charge_selected_manually_->unselect_all();
    this->charge_selected_automatically_->unselect_all();
}
void LOEF_drawer::keyPressEvent(QKeyEvent *ev) {
    if (ev->key() == Qt::Key_Control) {
        this->is_multi_selecting = true;
        qDebug("enable multiselect");
    } else {
        QWidget::keyPressEvent(ev);
    }
}
void LOEF_drawer::keyReleaseEvent(QKeyEvent *ev) {
    if (ev->key() == Qt::Key_Control) {
        this->is_multi_selecting = false;
        qDebug("disable multiselect");
    } else {
        QWidget::keyPressEvent(ev);
    }
}
// lazy
void LOEF_drawer::set_electric_potential(LOEF::experimental::electric_potential *of_parent) {
    this->electric_potential_handler = of_parent;
}
void LOEF_drawer::set_is_ready_made_requested(bool *of_parent) { this->is_ready_made_requested = of_parent; }
QImage LOEF_drawer::prepare_electric_potential_image() {
    auto width = this->width();
    auto height = this->height();
    qDebug() << "width=" << width << " , height=" << height;
    QImage result(width, height, QImage::Format_ARGB32);
    if (this->electric_potential_handler->surface_enabled &&
        this->electric_potential_handler->distance == 0.0 * LOEF::boostunits::volt) {
        double distance = 0.0;
        while (distance == 0.0) {
            /* it seems this QInputDialog disables the painter and cause segfault
             * so, this code is , actually, "crashes if distance is 0"
             * to avoid it, you must set distance with the menu before enable surface
             * */
            distance =
                QInputDialog::getDouble(nullptr, tr("distance cannot be zero!"), tr("enter non zero distance"), 0, 0);
        }
        this->electric_potential_handler->distance = distance * LOEF::boostunits::volt;
    }
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            LOEF::vec2d current_position(QPoint(x, y), this->dpmm_);
            LOEF::volt_quantity potential = 0.0 * LOEF::experimental::V;
            QColor current_color("white");
            auto max_abs_positive = this->electric_potential_handler->get_current_max_abs_positive();
            auto max_abs_negative = this->electric_potential_handler->get_current_max_abs_negative();
            for (const auto &fixed_charge_tuple : this->fixed_charges_) {
                const auto &fixed_charge = fixed_charge_tuple.second;
                auto charge_to_pos = fixed_charge.position() - current_position;
                if (charge_to_pos.length() <= LOEF::radius::FIXED) {
                    result.setPixel(x, y, QColor("transparent").rgba());
                    goto CONTINUE_TO_NEXT_POS;
                }
                potential += LOEF::experimental::k0 * fixed_charge.quantity() /
                             static_cast<LOEF::metre_quantity>(charge_to_pos.length());
            }
            if (this->electric_potential_handler->surface_enabled) {
                auto remainder = boost::units::fmod(potential, this->electric_potential_handler->distance).value();
                if (LOEF::experimental::is_about_same(remainder, 0.0, LOEF::experimental::max_error_surface)) {
                    current_color = QColor("black");
                    result.setPixel(x, y, current_color.rgb());
                    goto CONTINUE_TO_NEXT_POS;
                }
            }
            if (potential >= 0.0 * LOEF::experimental::V) {
                auto diff = 0xff * (potential / max_abs_positive);
                current_color = QColor(0xff, 0xff - diff, 0xff - diff);
            } else {
                auto diff = 0xff * (potential / max_abs_negative);
                current_color = QColor(0xff - diff, 0xff - diff, 0xff);
            }
            result.setPixel(x, y, current_color.rgb());
        CONTINUE_TO_NEXT_POS:;
        }
    }
    return result;
}
std::vector<LOEF::fixed_charge> LOEF_drawer::get_fixed_charges() {
    decltype(get_fixed_charges()) result;
    for (const auto &fixed_charge_tuple : fixed_charges_) {
        result.push_back(fixed_charge_tuple.second);
    }
    return result;
}
// end lazy
