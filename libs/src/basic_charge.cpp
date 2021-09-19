#include "basic_charge.hpp"

#include "vec2d.hpp"
namespace LOEF {
basic_charge_::basic_charge_(coulomb_value initial_quantity, millimetre_value initial_x, millimetre_value initial_y)
    : quantity_(initial_quantity), position_(vec2d(initial_x, initial_y)) {}
basic_charge_::basic_charge_(coulomb_value initial_quantity, vec2d initial_position)
    : quantity_(initial_quantity), position_(initial_position) {}
coulomb_value basic_charge_::quantity() const noexcept { return this->quantity_; }
vec2d basic_charge_::position() const noexcept { return this->position_; }
}  // namespace LOEF
