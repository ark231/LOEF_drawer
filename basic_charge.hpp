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
    double quantity_ = 0;
    vec2d position_;

   public:
    basic_charge_(double initial_quantity, int initial_x, int initial_y);
    basic_charge_(double initial_quantity, vec2d initial_position);
    double quantity() const noexcept;
    vec2d position() const noexcept;
};

}  // namespace LOEF
#endif
