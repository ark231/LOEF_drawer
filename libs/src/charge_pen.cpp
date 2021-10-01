#include "charge_pen.hpp"
namespace LOEF {
#ifdef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
charge_pen::charge_pen(bool is_positive, vec2d initial_position, millimetre_quantity interval, int max_x, int max_y,
                       dot_per_millimetre_quantity dpmm)
    : is_positive_(is_positive),
      position_(initial_position),
      interval_(interval),
      max_x(max_x),
      max_y(max_y),
      path(new LOEF_path) {
    path->moveTo((initial_position.to_QPoint(dpmm)));
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
    this->position_ += normalize(electric_field) * (interval_ / millimetre);
    this->previous_delta_position_ = normalize(electric_field) * (interval_ / millimetre);
    /*
    qDebug() << "    @charge_pen step_forward "
             << "pos:" << this->position_
             << "delta_posiiton:" << (normalize(electric_field) * (interval_ / millimetre));
             */
    path->lineTo((this->position_).to_QPoint(dpmm));
    if ((0 <= position_.x() * dpmm && position_.x() * dpmm <= max_x) &&
        (0 <= position_.y() * dpmm && position_.y() * dpmm <= max_y)) {
        for (auto iterator = begin; iterator != end; iterator++) {
            if (iterator->second.quantity() / boostunits::coulomb == 0) {
                continue;  //中性電荷は突き抜ける
            }
            if ((iterator->second.position() - position_).length() < (radius::FIXED + 1.0 * millimetre)) {
                return step_status::FINISH;
            }
        }
        return step_status::CONTINUE;
    } else {
        return step_status::FINISH;
    }
}
}  // namespace LOEF
