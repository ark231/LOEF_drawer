#include "charge_pen.hpp"
namespace LOEF {
#ifdef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
charge_pen::charge_pen(bool is_positive, vec2d initial_position, screen_mm interval, int max_x, int max_y)
    : is_positive_(is_positive),
      position_(initial_position),
      interval_(interval),
      max_x(max_x),
      max_y(max_y),
      path(new LOEF_path) {
    path->moveTo(static_cast<QPoint>(initial_position));
}

std::shared_ptr<LOEF_path> charge_pen::get_path() { return path; }
#endif
template <class fixed_charge_map_iterator_>
bool charge_pen::step_forward(fixed_charge_map_iterator_ begin, fixed_charge_map_iterator_ end, double dpmm) {
    vec2d electric_field(0, 0);
    for (auto fixed_charge_itr = begin; fixed_charge_itr != end; fixed_charge_itr++) {
        auto fixed_charge = fixed_charge_itr->second;
        auto charge_to_pen = this->position_ - fixed_charge.position();
        /*電場の向きさえわかればいいので、長さが定数倍されていても問題ない
         * クーロンの法則の係数やらドット単位の長さは大きすぎることが多いため、小さくする
         * 今回は、必ず固定電荷からペンに向かうベクトルに対して長さを掛けているので、固定電荷の符号とベクトルの向きが偶然一致しているため符号付きのまま計算に使用している
         */

        electric_field += normalize(charge_to_pen) * (fixed_charge.quantity() / (charge_to_pen.length_square() / dpmm));
    }
    this->position_ += normalize(electric_field) * (interval_ * dpmm);
    path->lineTo(static_cast<QPoint>(this->position_));
    if ((0 <= position_.x() && position_.x() <= max_x) && (0 <= position_.y() && position_.y() <= max_y)) {
        for (auto iterator = begin; iterator != end; iterator++) {
            if (iterator->second.quantity() == 0) {
                continue;  //中性電荷は突き抜ける
            }
            if ((iterator->second.position() - position_).length() < (radius::FIXED + 1) * dpmm) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}
}  // namespace LOEF
