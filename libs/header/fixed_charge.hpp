#ifndef LOEF_DRAWER_FIXED_CHARGE
#define LOEF_DRAWER_FIXED_CHARGE
#include <unordered_map>
#include <vector>

#include "basic_charge.hpp"
#include "vec2d.hpp"
namespace LOEF {
class fixed_charge : public basic_charge_ {
   public:
    using basic_charge_::basic_charge_;
    template <class fixed_charge_containter_itr>
    std::vector<vec2d> calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                         int num_result, int num_sample) const;
};
}  // namespace LOEF
#ifndef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
#include "fixed_charge.cpp"
#endif
#endif
