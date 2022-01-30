#ifndef LOEF_DRAWER_LAZY_HELPER
#define LOEF_DRAWER_LAZY_HELPER
#include <QWidget>

#include "units.hpp"
#include "vec2d.hpp"
namespace LOEF {
namespace experimental {
QWidget* search_for_origin(QWidget* decendant);
bool is_about_same(double v1, double v2, double max_error);
bool is_about_same_rel(double v1, double v2, double max_error);
template <class MapIterator>
electric_field calc_electric_field(vec2d pos, MapIterator fixed_begin, MapIterator fixed_end) {
    LOEF::electric_field electric_field(0, 0);
    for (auto fixed_charge_itr = fixed_begin; fixed_charge_itr != fixed_end; fixed_charge_itr++) {
        auto fixed_charge = fixed_charge_itr->second;
        vec2d charge_to_pos = pos - fixed_charge.position();
        electric_field +=
            normalize(charge_to_pos).to_dimentionless() * (fixed_charge.quantity() / (charge_to_pos.length_square()));
    }
    return electric_field;
}
}  // namespace experimental
}  // namespace LOEF
#endif
