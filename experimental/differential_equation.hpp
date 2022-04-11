#ifndef LOEF_DRAWER_DIFFERENTIAL_EQUATION
#define LOEF_DRAWER_DIFFERENTIAL_EQUATION
#include <array>

#include "experimental_helper.hpp"
#include "vec2d.hpp"
namespace LOEF {
namespace experimental {
namespace vecElem {
constexpr char X = 0;
constexpr char Y = 1;
}  // namespace vecElem
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
}  // namespace experimental
}  // namespace LOEF
#endif
