#ifndef LOEF_DRAWER_FIXED_CHARGE
#define LOEF_DRAWER_FIXED_CHARGE
#include <unordered_map>
#include <vector>

#include "basic_charge.hpp"
#include "vec2d.hpp"
namespace LOEF {
class fixed_charge : public basic_charge_ {
    //個別にリセットすると不揃いになるので
    template <class fixed_charge_containter_itr>
    friend void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end);

   public:
    using basic_charge_::basic_charge_;
    template <class fixed_charge_containter_itr>
    std::vector<vec2d> calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                         int num_result, int num_sample) const;
    /**
     * @brief ペンが到着したときに呼び出す
     *
     * @param offset 到着したペンとこの電荷の位置関係
     *
     * @return そのペンは到着しても良いかどうか
     * @retval false 定員オーバーで、到着してはならないはずである
     * @retval true 到着して良いはずである
     */
    bool pen_arrive(vec2d offset, decltype(1.0 / boostunits::coulomb) inverse_permittivity);
    bool needs_pens(decltype(1.0 / boostunits::coulomb) inverse_permittivity);

   private:
    std::vector<vec2d> offsets_enterd_pen_;
};
template <class fixed_charge_containter_itr>
void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end);
}  // namespace LOEF
#ifndef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
#include "fixed_charge.cpp"
#endif
#endif
