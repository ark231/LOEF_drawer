#include "electric_potential.hpp"

#include "general_consts.hpp"
namespace LOEF {
namespace experimental {
volt_quantity electric_potential::get_current_max_abs_positive() { return this->current_max_abs_positive; }
volt_quantity electric_potential::get_current_max_abs_negative() { return this->current_max_abs_negative; }
void electric_potential::set_current_max_abs_positive(volt_quantity new_value) {
    this->current_max_abs_positive = new_value;
}
void electric_potential::set_current_max_abs_negative(volt_quantity new_value) {
    this->current_max_abs_negative = new_value;
}
void electric_potential::hint_add_charge(coulomb_quantity new_charge) {
    /*電荷の描画時の円の外周部分での電位が描画範囲でのその電荷の作る最大の電位*/
    volt_quantity possible_max = k0 * new_charge / static_cast<decltype(1.0 * boostunits::metre)>(radius::FIXED);
    if (possible_max > 0.0 * V) {
        this->current_max_abs_positive += possible_max;
    } else {
        this->current_max_abs_negative += possible_max;
    }
}
void electric_potential::hint_delete_charge(coulomb_quantity deleted_charge) {
    volt_quantity possible_max = k0 * deleted_charge / static_cast<decltype(1.0 * boostunits::metre)>(radius::FIXED);
    if (possible_max > 0.0 * V) {
        this->current_max_abs_positive -= possible_max;
    } else {
        this->current_max_abs_negative -= possible_max;
    }
}
}  // namespace experimental
}  // namespace LOEF
