#include "electric_potential.hpp"

#include <QImage>
#include <QInputDialog>
#include <QLineF>
#include <QVector>
#include <optional>
#include <utility>
#include <vector>

#include "experimental_helper.hpp"
#include "general_consts.hpp"
namespace LOEF {
namespace experimental {
volt_quantity electric_potential_handler::get_current_max_abs_positive() { return this->current_max_abs_positive; }
volt_quantity electric_potential_handler::get_current_max_abs_negative() { return this->current_max_abs_negative; }
void electric_potential_handler::set_current_max_abs_positive(volt_quantity new_value) {
    this->current_max_abs_positive = new_value;
}
void electric_potential_handler::set_current_max_abs_negative(volt_quantity new_value) {
    this->current_max_abs_negative = new_value;
}
void electric_potential_handler::hint_add_charge(coulomb_quantity new_charge) {
    /*電荷の描画時の円の外周部分での電位が描画範囲でのその電荷の作る最大の電位*/
    volt_quantity possible_max = k0 * new_charge / static_cast<decltype(1.0 * boostunits::metre)>(radius::FIXED);
    if (possible_max > 0.0 * V) {
        this->current_max_abs_positive += possible_max;
    } else {
        this->current_max_abs_negative += possible_max;
    }
}
void electric_potential_handler::hint_delete_charge(coulomb_quantity deleted_charge) {
    volt_quantity possible_max = k0 * deleted_charge / static_cast<decltype(1.0 * boostunits::metre)>(radius::FIXED);
    if (possible_max > 0.0 * V) {
        this->current_max_abs_positive -= possible_max;
    } else {
        this->current_max_abs_negative -= possible_max;
    }
}
std::variant<QImage, electric_potential_handler::PaintImageSkipped_, electric_potential_handler::PaintAborted_>
electric_potential_handler::prepare_electric_potential_image_(
    std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges, LOEF::dots_quantity width,
    LOEF::dots_quantity height, LOEF::dot_per_millimetre_quantity dpmm) {
    if (fixed_charges.empty()) {
        return PaintImageSkipped_();
    }
    bool equipotential_line_drawing_is_skipped = not(this->algorithm_type == ep_algorithm_type::original);
    QImage potential_image = QImage(width, height, QImage::Format_ARGB32);
    if (this->surface_enabled && this->distance == 0.0 * LOEF::boostunits::volt) {
        // NOTE: アルゴリズムに関係なく間隔ゼロでは真っ黒になってしまう(どころかバグる)ので
        double distance = 0.0;
        while (distance == 0.0) {
            distance =
                QInputDialog::getDouble(nullptr, tr("distance cannot be zero!"), tr("enter non zero distance"), 0, 0);
        }
        this->distance = distance * LOEF::boostunits::volt;
        return PaintAborted_();
    }
    if (equipotential_line_drawing_is_skipped && not this->color_enabled) {
        return PaintImageSkipped_();
    }
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            LOEF::vec2d current_position(QPoint(x, y), dpmm);
            LOEF::volt_quantity potential = 0.0 * LOEF::experimental::V;
            QColor current_color("white");
            auto max_abs_positive = this->get_current_max_abs_positive();
            auto max_abs_negative = this->get_current_max_abs_negative();
            for (const auto &fixed_charge_tuple : fixed_charges) {
                const auto &fixed_charge = fixed_charge_tuple.second;
                auto charge_to_pos = fixed_charge.position() - current_position;
                if (charge_to_pos.length() <= LOEF::radius::FIXED) {
                    potential_image.setPixel(x, y, QColor("transparent").rgba());
                    goto CONTINUE_TO_NEXT_POS;
                }
                potential += LOEF::experimental::k0 * fixed_charge.quantity() /
                             static_cast<LOEF::metre_quantity>(charge_to_pos.length());
            }
            if (not equipotential_line_drawing_is_skipped && this->surface_enabled) {
                auto remainder = boost::units::fmod(potential, this->distance).value();
                if (LOEF::experimental::is_about_same(remainder, 0.0, this->max_error_surface)) {
                    current_color = QColor("black");
                    potential_image.setPixel(x, y, current_color.rgb());
                    goto CONTINUE_TO_NEXT_POS;  //ここに関しては普通のcontinueでもいいが、上と揃えておく
                }
            }
            if (this->color_enabled) {
                if (potential >= 0.0 * LOEF::experimental::V) {
                    auto diff = 0xff * (potential / max_abs_positive);
                    current_color = QColor(0xff, 0xff - diff, 0xff - diff);
                } else {
                    auto diff = 0xff * (potential / max_abs_negative);
                    current_color = QColor(0xff - diff, 0xff - diff, 0xff);
                }
            }
            potential_image.setPixel(x, y, current_color.rgb());
        CONTINUE_TO_NEXT_POS:;
        }
    }
    return potential_image;
}

// これはスレッドセーフのはずなので、マルチスレッド化しやすいはず
QVector<QLineF> electric_potential_handler::find_equipotential_lines_at_(
    millimetre_quantity x, millimetre_quantity y, std::unordered_map<id_type, fixed_charge> fixed_charges,
    dot_per_millimetre_quantity dpmm) {
    vec2d top_left = {x, y};
    vec2d top_right = {x + this->square_side_length, y};
    vec2d bottom_right = {x + this->square_side_length, y + this->square_side_length};
    vec2d bottom_left = {x, y + this->square_side_length};
    for (const auto &vertex : {top_left, top_right, bottom_right, bottom_left}) {
        auto nearest_charge_id = find_closest(fixed_charges.cbegin(), fixed_charges.cend(), vertex);
        auto vertex_to_nearest = fixed_charges[nearest_charge_id].position() - vertex;
        if (vertex_to_nearest.length() < radius::FIXED) {
            return QVector<QLineF>();  //さもなくば、無限本の等電位線を描こうとして無限ループの危険性がある
        }
    }
    QVector<QLineF> result;
    volt_quantity top_left_potential = calc_electric_potential(top_left, fixed_charges.cbegin(), fixed_charges.cend());
    volt_quantity top_right_potential =
        calc_electric_potential(top_right, fixed_charges.cbegin(), fixed_charges.cend());
    volt_quantity bottom_right_potential =
        calc_electric_potential(bottom_right, fixed_charges.cbegin(), fixed_charges.cend());
    volt_quantity bottom_left_potential =
        calc_electric_potential(bottom_left, fixed_charges.cbegin(), fixed_charges.cend());
    volt_quantity max_potential = std::max({top_left_potential.value(), top_right_potential.value(),
                                            bottom_right_potential.value(), bottom_left_potential.value()}) *
                                  V;
    volt_quantity min_potential = std::min({top_left_potential.value(), top_right_potential.value(),
                                            bottom_right_potential.value(), bottom_left_potential.value()}) *
                                  V;
    // qDebug() << calc_nearest_multiple(min_potential, distance)
    //          << calc_nearest_multiple(max_potential, distance);
    for (auto target_potential = calc_nearest_multiple(min_potential, distance);
         target_potential <= calc_nearest_multiple(max_potential, distance); target_potential += distance) {
        // qDebug() << target_potential;
        std::vector<vec2d> found_crosssections;
        std::vector<std::pair<vec2d, volt_quantity>> potential_data = {{top_left, top_left_potential},
                                                                       {top_right, top_right_potential},
                                                                       {bottom_right, bottom_right_potential},
                                                                       {bottom_left, bottom_left_potential}};
        for (auto i = 0; i < 4; i++) {
            vec2d crosssection_candidate = LOEF::experimental::inverse_linear_interpolation(
                potential_data[i], potential_data[(i + 1) % 4], target_potential);
            if (is_in_between(potential_data[i].first, crosssection_candidate, potential_data[(i + 1) % 4].first)) {
                found_crosssections.push_back(crosssection_candidate);
            }
            // qDebug() << found_crosssections.size();
            switch (found_crosssections.size()) {
                case 0:
                    break;
                case 1:
                    // painter.drawPoint(found_crosssections[0].to_QPointF(dpmm));
                    // TODO:点のサイズを確認
                    break;
                case 2:
                    // qDebug() << found_crosssections[0].to_QPointF(dpmm);
                    result.push_back(
                        QLineF(found_crosssections[0].to_QPointF(dpmm), found_crosssections[1].to_QPointF(dpmm)));
                    // qDebug() << "end drawing";
                default:
                    // TODO: 3点以上見つかった時の扱いについて考察
                    break;
            }
        }
    }
    return result;
}

bool electric_potential_handler::draw_electric_potential_image(
    QPainter &painter, std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges, LOEF::dots_quantity width,
    LOEF::dots_quantity height, LOEF::dot_per_millimetre_quantity dpmm) {
    if (this->color_enabled || this->surface_enabled) {
        qDebug() << "width=" << width << " , height=" << height << " ,dpmm=" << dpmm;
        auto potential_image = prepare_electric_potential_image_(fixed_charges, width, height, dpmm);
        if (std::holds_alternative<PaintAborted_>(potential_image)) {
            return false;
        }
        if (std::holds_alternative<QImage>(potential_image)) {
            painter.drawImage(0, 0, std::get<QImage>(potential_image));
        }
        if (this->surface_enabled) {
            switch (this->algorithm_type) {
                case ep_algorithm_type::original:
                    qDebug() << "original";
                    // prepare~ ですでに描画済み
                    break;
                case ep_algorithm_type::marching_square:
                    qDebug() << "marching square!";
                    QVector<QLineF> equipotential_lines;
                    equipotential_lines.reserve(((height / dpmm) * (width / dpmm)).value());  //過大評価か？
                    for (millimetre_quantity y = 0 * mm; y < height / dpmm; y += this->square_side_length) {
                        for (millimetre_quantity x = 0 * mm; x < width / dpmm; x += this->square_side_length) {
                            auto found_ep_lines = find_equipotential_lines_at_(x, y, fixed_charges, dpmm);
                            for (const auto &found_ep_line : found_ep_lines) {
                                equipotential_lines.push_back(found_ep_line);
                            }
                        }
                    }
                    painter.drawLines(equipotential_lines);
                    break;
                    return true;
            }
        }
    }
    return false;
}
}  // namespace experimental
}  // namespace LOEF
