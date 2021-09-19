#ifndef LOEF_DRAWER_BASIC_CHARGE
#define LOEF_DRAWER_BASIC_CHARGE
#include "vec2d.hpp"
namespace LOEF {
/**
 * @brief basic charge class
 * this class is immutable and therefore read-only
 */
class basic_charge_ {
   protected:
    coulomb_quantity quantity_ = 0;
    vec2d position_;

   public:
    basic_charge_(coulomb_quantity initial_quantity, millimetre_quantity initial_x, millimetre_quantity initial_y);
    basic_charge_(coulomb_quantity initial_quantity, vec2d initial_position);
    basic_charge_() {}
    coulomb_quantity quantity() const noexcept;
    vec2d position() const noexcept;
};

}  // namespace LOEF
#endif
