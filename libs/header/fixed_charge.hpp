#ifndef LOEF_DRAWER_FIXED_CHARGE
#define LOEF_DRAWER_FIXED_CHARGE
#include <memory>
#include <unordered_map>
#include <vector>

#include "basic_charge.hpp"
#include "general_consts.hpp"
#include "vec2d.hpp"
namespace LOEF {
class fixed_charge : public basic_charge_ {
    //個別にリセットすると不揃いになるので
    template <class fixed_charge_containter_itr>
    friend void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end);

   public:
    fixed_charge();
    fixed_charge(coulomb_quantity initial_quantity, millimetre_quantity initial_x, millimetre_quantity initial_y);
    fixed_charge(coulomb_quantity initial_quantity, vec2d initial_position);
    template <class fixed_charge_containter_itr>
    std::vector<vec2d> calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                         unsigned int num_result, unsigned int num_sample) const;
    /**
     * @brief ペンが到着したときに呼び出す
     *
     * @param offset 到着したペンとこの電荷の位置関係
     *
     * @return そのペンは到着しても良いかどうか
     * @retval false 定員オーバーで、到着してはならないはずである
     * @retval true 到着して良いはずである
     */
    bool pen_arrive(vec2d offset);
    bool needs_pens() const;
    size_t num_needed_pens() const;
    void update_inverse_permittivity(inverse_permittivity_quantity);

   private:
    //描画時にコピーしても共有されるように。全体をポインタにするのは修正箇所が多すぎたのでやめた
    std::shared_ptr<std::vector<vec2d>> offsets_entered_pen_;
    inverse_permittivity_quantity inverse_permittivity_ = LOEF::initial_inverse_permittivity;
};
template <class fixed_charge_containter_itr>
void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end);
}  // namespace LOEF
#ifndef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
#    include "fixed_charge.cpp"
#endif
#endif
