#ifndef LOEF_DRAWER_DIFFERENTIAL_EQUATION
#define LOEF_DRAWER_DIFFERENTIAL_EQUATION
#include <QThreadStorage>
#include <array>
#include <unordered_map>

#include "charge_pen.hpp"
#include "experimental_helper.hpp"
#include "fixed_charge.hpp"
#include "vec2d.hpp"
namespace LOEF {
namespace experimental {
namespace vecElem {
constexpr char X = 0;
constexpr char Y = 1;
}  // namespace vecElem
// lazy
/**
 * @brief ペンが入った電荷の情報を格納する
 * @details integrate()から例外で抜ける時にこれを投げる。呼び出し側でこれを受け取って、返す
 *
 */
struct EnteredChargeInfo {
    bool is_entered = false;
    id_type id;
    vec2d pen_final_pos;
};
// end lazy
template <class MapIterator>
class LOEF_system {
   public:
    using state_type = std::array<double, 2>;
    LOEF_system(MapIterator begin, MapIterator end, bool is_positive)
        : fixed_begin_(begin), fixed_end_(end), is_positive_(is_positive) {}
    void operator()(const state_type& P, state_type& dPdt, const double) {
        auto electric_field = normalize(calc_electric_field(vec2d(P), fixed_begin_, fixed_end_));
        if (not is_positive_) {
            electric_field *= -1.0;  //負電荷は電場と逆向きに進む
        }
        dPdt[vecElem::X] = electric_field.x().value();
        dPdt[vecElem::Y] = electric_field.y().value();
    }

   private:
    MapIterator fixed_begin_;
    MapIterator fixed_end_;
    bool is_positive_;
};

class differential_equation_handler {
   public:
    enum class total_polarity { positive, negative, equal, default_value };

   private:
    using fixed_charge_map_const_iter = std::unordered_map<id_type, fixed_charge>::const_iterator;
    total_polarity fixed_charges_total_polarity_ = total_polarity::default_value;
    fixed_charge_map_const_iter fixed_begin_;
    fixed_charge_map_const_iter fixed_end_;
    dot_per_millimetre_quantity dpmm_;
    void update_fixed_charges_total_polarity();
    void observe(const LOEF_system<fixed_charge_map_const_iter>::state_type& state, const double) const;
    auto create_observer() const {
        return [this](const LOEF_system<fixed_charge_map_const_iter>::state_type& state, const double arg) {
            this->observe(state, arg);
        };
    }
    mutable QThreadStorage<std::pair<id_type, charge_pen>> current_pen;
    mutable QThreadStorage<bool> current_is_every_charge_divergent;

   public:
    total_polarity set_fixed_charges(const std::unordered_map<id_type, fixed_charge>& fixed_charges);
    void set_dpmm(dot_per_millimetre_quantity dpmm);
    EnteredChargeInfo integrate_pen(std::pair<id_type, charge_pen> item) const;
};
}  // namespace experimental
}  // namespace LOEF
#endif
