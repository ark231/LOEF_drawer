#include "fixed_charge.hpp"
namespace LOEF {
template <class fixed_charge_containter_itr>
std::vector<vec2d> calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end, int num_div) {
    return std::vector<vec2d>(1, vec2d(0, 0));
}
}  // namespace LOEF
