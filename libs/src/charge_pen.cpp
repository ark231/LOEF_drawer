#include "charge_pen.hpp"
namespace LOEF {
#ifdef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
void LOEF_path::set_is_positive(bool new_value) noexcept { this->is_positive_ = new_value; }
bool LOEF_path::is_positive() const noexcept { return this->is_positive_; }

charge_pen::charge_pen(bool is_positive, vec2d initial_position, millimetre_quantity interval, int max_x, int max_y,
                       dot_per_millimetre_quantity dpmm)
    : is_positive_(is_positive),
      position_(initial_position),
      interval_(interval),
      max_x(max_x),
      max_y(max_y),
      path(new LOEF_path) {
    path->moveTo((initial_position.to_QPoint(dpmm)));
    path->set_is_positive(is_positive);
}

std::shared_ptr<LOEF_path> charge_pen::get_path() { return path; }
#endif
template <class fixed_charge_map_iterator_>
step_status charge_pen::step_forward(fixed_charge_map_iterator_ begin, fixed_charge_map_iterator_ end,
                                     dot_per_millimetre_quantity dpmm) {
    vec2d electric_field(0, 0);
    for (auto fixed_charge_itr = begin; fixed_charge_itr != end; fixed_charge_itr++) {
        auto fixed_charge = fixed_charge_itr->second;
        vec2d charge_to_pen = this->position_ - fixed_charge.position();
        /*電場の向きさえわかればいいので、長さが定数倍されていても問題ない
         * クーロンの法則の係数は大きすぎるため、小さくする
         * 今回は、必ず固定電荷からペンに向かうベクトルに対して長さを掛けているので、固定電荷の符号とベクトルの向きが偶然一致しているため符号付きのまま計算に使用している
         */

        electric_field += normalize(charge_to_pen) * ((fixed_charge.quantity() / boostunits::coulomb) /
                                                      ((charge_to_pen.length_square() / millimetre / millimetre)));
    }
    if (fuzzy_compare(this->previous_delta_position_, -normalize(electric_field) * (interval_ / millimetre))) {
        return step_status::ABORT;  // or will loop forever!
    }
    if (this->is_positive_) {
        this->position_ += normalize(electric_field) * (interval_ / millimetre);
    } else {  // negative
        this->position_ += -normalize(electric_field) * (interval_ / millimetre);
    }
    this->previous_delta_position_ = normalize(electric_field) * (interval_ / millimetre);
    path->lineTo((this->position_).to_QPoint(dpmm));
    if ((0 <= position_.x() * dpmm && position_.x() * dpmm <= max_x) &&
        (0 <= position_.y() * dpmm && position_.y() * dpmm <= max_y)) {
        for (auto fixed_iterator = begin; fixed_iterator != end; fixed_iterator++) {
            if (fixed_iterator->second.quantity() / boostunits::coulomb == 0) {
                continue;  //中性電荷は突き抜ける
            }
            if ((fixed_iterator->second.position() - position_).length() < (radius::FIXED + 1.0 * millimetre)) {
                if (fixed_iterator->second.pen_arrive(this->position_ - fixed_iterator->second.position())) {
                    return step_status::FINISH;
                } else {
                    return step_status::ABORT;
                }
            }
        }
        return step_status::CONTINUE;
    } else {
        return step_status::FINISH;
    }
}
}  // namespace LOEF
