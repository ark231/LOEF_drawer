#include "loef_drawer.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QtGlobal>
#include <iterator>

#include "LOEF_QPainter.hpp"
#include "units.hpp"
namespace LOEF {
class state_charge_selected_ {
    bool exists_selected_ = false;
    LOEF::id_type id_selected_charge_ = -1;  // this must be super big number ,as so much instances can't be stored
    vec2d offset_ = {0, 0};

   public:
    state_charge_selected_();
    operator bool();
    void set_selected(size_t idx, vec2d offset);
    void unselected();
    LOEF::id_type get_selected();
    vec2d get_offset();
};
state_charge_selected_::state_charge_selected_() {
    exists_selected_ = false;
    id_selected_charge_ = -1;
    offset_ = {0, 0};
}
state_charge_selected_::operator bool() { return exists_selected_; }
void state_charge_selected_::set_selected(LOEF::id_type id, vec2d offset) {
    exists_selected_ = true;
    id_selected_charge_ = id;
    offset_ = offset;
}
void state_charge_selected_::unselected() {
    exists_selected_ = false;
    id_selected_charge_ = -1;
}
LOEF::id_type state_charge_selected_::get_selected() { return id_selected_charge_; }
vec2d state_charge_selected_::get_offset() { return offset_; }

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
    charge_selected_ = new LOEF::state_charge_selected_;
    fixed_charge_id_handler_ = new LOEF::id_handler;
    charge_pen_id_handler_ = new LOEF::id_handler;
    fixed_charges_[fixed_charge_id_handler_->new_id()] =
        LOEF::fixed_charge(+1 * LOEF::boostunits::coulomb, 200.0 / dpmm_, 300.0 / dpmm_);
    fixed_charges_[fixed_charge_id_handler_->new_id()] =
        LOEF::fixed_charge(+1 * LOEF::boostunits::coulomb, 300.0 / dpmm_, 300.0 / dpmm_);
    fixed_charges_[fixed_charge_id_handler_->new_id()] =
        LOEF::fixed_charge(+0 * LOEF::boostunits::coulomb, 400.0 / dpmm_, 300.0 / dpmm_);
    fixed_charges_[fixed_charge_id_handler_->new_id()] =
        LOEF::fixed_charge(-2 * LOEF::boostunits::coulomb, 600.0 / dpmm_, 300.0 / dpmm_);
}
void LOEF_drawer::paintEvent(QPaintEvent *) {
    LOEF::painter painter(this);
    painter.set_resolution(dpmm_);
    auto width = this->width();
    auto height = this->height();
    painter.drawLine(0, height / 2.0, width, height / 2.0);
    painter.drawLine(width / 2.0, 0, width / 2.0, height);
    qDebug("@LOEF_drawer paintEvent:fixed_charge loop start");
    for (const auto &charge : fixed_charges_) {
        painter.draw_fixed_charge(charge.second);
        if (charge.second.quantity() / LOEF::boostunits::coulomb > 0) {
            qDebug("@LOEF_drawer paintEvent:positive charge");
            for (const auto &position : charge.second.calc_pen_init_pos(
                     fixed_charges_.begin(), fixed_charges_.end(),
                     std::abs(charge.second.quantity() / LOEF::boostunits::coulomb) * 12, 360)) {
                auto pen_id = charge_pen_id_handler_->new_id();
                charge_pens_[pen_id] = LOEF::charge_pen(true, position, 1 * LOEF::millimetre, width, height, dpmm_);
                charge_paths_[pen_id] = charge_pens_[pen_id].get_path();
                qDebug() << "@LOEF_drawer paintEvent:new charge_pen id:" << pen_id;
            }
            std::vector<LOEF::id_type> ids_to_erase;
            qDebug("@LOEF_drawer paintEvent charge_pen loop start");
            while (!charge_pens_.empty()) {
                for (auto charge_pen = charge_pens_.begin(); charge_pen != charge_pens_.end(); charge_pen++) {
                    // qDebug() << "    @LOEF_drawer paintEvent call step_forward id:" << charge_pen->first;
                    switch (charge_pen->second.step_forward(fixed_charges_.begin(), fixed_charges_.end(), dpmm_)) {
                        case LOEF::step_status::FINISH:
                            ids_to_erase.push_back(charge_pen->first);
                            break;
                        case LOEF::step_status::ABORT:
                            ids_to_erase.push_back(charge_pen->first);
                            break;
                        case LOEF::step_status::CONTINUE:
                            break;
                    }
                }
                for (const auto &id_to_erase : ids_to_erase) {
                    qDebug() << "@LOEF_drawer paintEvent erase charge_pen id:" << id_to_erase;
                    auto iterator_to_erase = charge_pens_.find(id_to_erase);
                    if (iterator_to_erase != charge_pens_.end()) {
                        charge_pens_.erase(iterator_to_erase);
                    }
                }
                ids_to_erase.clear();
            }
            qDebug("@LOEF_drawer paintEvent charge_pen loop end");

            qDebug("@LOEF_drawer paintEvent charge_path loop begin");
            for (auto charge_path = charge_paths_.begin(); charge_path != charge_paths_.end(); charge_path++) {
                painter.draw_LOEF_path(*(charge_path->second));
            }
            qDebug("@LOEF_drawer paintEvent charge_path loop end");
        }
    }
    qDebug("@LOEF_drawer paintEvent:fixed_charge loop end");
}
void LOEF_drawer::mousePressEvent(QMouseEvent *ev) {
    LOEF::vec2d pos_mouse(ev->pos(), dpmm_);
    for (auto charge = fixed_charges_.begin(); charge != fixed_charges_.end(); charge++) {
        LOEF::vec2d offset = charge->second.position() - pos_mouse;
        if (offset.length() * dpmm_ <= LOEF::radius::FIXED * dpmm_) {
            charge_selected_->set_selected(charge->first, offset);
        }
    }
}
void LOEF_drawer::mouseMoveEvent(QMouseEvent *ev) {
    LOEF::vec2d pos_mouse(ev->pos(), dpmm_);
    if (*charge_selected_) {
        auto id_selected = charge_selected_->get_selected();
        auto new_pos = pos_mouse + charge_selected_->get_offset();
        replace_fixed_charge(id_selected, std::nullopt, new_pos);
        emit fixed_charge_position_changed(id_selected, new_pos.x(), new_pos.y());
    }
}
void LOEF_drawer::mouseReleaseEvent(QMouseEvent *) { charge_selected_->unselected(); }
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
