#ifndef LOEF_DRAWER_DIFFERENTIAL_EQUATION
#define LOEF_DRAWER_DIFFERENTIAL_EQUATION
#include <array>

#include "lazy_helper.hpp"
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
    LOEF_system(MapIterator begin, MapIterator end) : fixed_begin(begin), fixed_end(end) {}
    void operator()(const state_type& P, state_type& dPdt, const double) {
        auto electric_field = calc_electric_field(vec2d(P), fixed_begin, fixed_end);
        dPdt[vecElem::X] = electric_field.x().value();
        dPdt[vecElem::Y] = electric_field.y().value();
    }

   private:
    MapIterator fixed_begin;
    MapIterator fixed_end;
};
}  // namespace experimental
}  // namespace LOEF
#endif
