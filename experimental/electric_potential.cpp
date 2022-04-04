#include "electric_potential.hpp"

#include <QImage>
#include <QInputDialog>
#include <optional>

#include "general_consts.hpp"
#include "lazy_helper.hpp"
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
std::optional<QImage> electric_potential_handler::prepare_electric_potential_image_(
    std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges, LOEF::dots_quantity width,
    LOEF::dots_quantity height, LOEF::dot_per_millimetre_quantity dpmm) {
    if (fixed_charges.empty()) {
        return std::nullopt;
    }
    std::optional<QImage> potential_image = QImage(width, height, QImage::Format_ARGB32);
    if (this->surface_enabled && this->distance == 0.0 * LOEF::boostunits::volt) {
        double distance = 0.0;
        while (distance == 0.0) {
            distance =
                QInputDialog::getDouble(nullptr, tr("distance cannot be zero!"), tr("enter non zero distance"), 0, 0);
        }
        this->distance = distance * LOEF::boostunits::volt;
        potential_image = std::nullopt;
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
                    potential_image->setPixel(x, y, QColor("transparent").rgba());
                    goto CONTINUE_TO_NEXT_POS;
                }
                potential += LOEF::experimental::k0 * fixed_charge.quantity() /
                             static_cast<LOEF::metre_quantity>(charge_to_pos.length());
            }
            if (this->surface_enabled) {
                auto remainder = boost::units::fmod(potential, this->distance).value();
                if (LOEF::experimental::is_about_same(remainder, 0.0, LOEF::experimental::max_error_surface)) {
                    current_color = QColor("black");
                    potential_image->setPixel(x, y, current_color.rgb());
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
            potential_image->setPixel(x, y, current_color.rgb());
        CONTINUE_TO_NEXT_POS:;
        }
    }
    return potential_image;
}

bool electric_potential_handler::draw_electric_potential_image(
    QPainter &painter, std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges, LOEF::dots_quantity width,
    LOEF::dots_quantity height, LOEF::dot_per_millimetre_quantity dpmm) {
    if (this->color_enabled || this->surface_enabled) {
        qDebug() << "width=" << width << " , height=" << height;
        auto potential_image = prepare_electric_potential_image_(fixed_charges, width, height, dpmm);
        if (potential_image) {
            painter.drawImage(0, 0, potential_image.value());
            return true;
        }
    }
    return false;
}
}  // namespace experimental
}  // namespace LOEF
