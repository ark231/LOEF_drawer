#ifndef LOEF_DRAWER_UNITS
#define LOEF_DRAWER_UNITS
#include <QDebug>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
namespace LOEF {
namespace boostunits = boost::units::si;
using millimetre_unit =
    boost::units::make_scaled_unit<boostunits::length,
                                   boost::units::scale<10, boost::units::static_rational<-3>>>::type;
inline millimetre_unit millimetre;
using millimetre_quantity = boost::units::quantity<millimetre_unit, double>;
using dot_per_millimetre_unit =
    boost::units::power_typeof_helper<millimetre_unit, boost::units::static_rational<-1>>::type;
inline dot_per_millimetre_unit dot_per_millimetre;
using dot_per_millimetre_quantity = boost::units::quantity<dot_per_millimetre_unit, double>;
using coulomb_quantity = boost::units::quantity<boost::units::si::electric_charge, double>;
using radian_quantity = boost::units::quantity<boost::units::si::plane_angle, double>;
using electric_field_strength_quantity =
    decltype(1.0 * boostunits::coulomb / (millimetre * millimetre));  //!クーロンの法則の変形から導かれる次元そのまま
inline auto electric_field_strength_unit_quantity = 1.0 * boostunits::coulomb / (millimetre * millimetre);
using dimensionless_quantity = boost::units::quantity<boost::units::si::dimensionless, double>;
using inverse_permittivity_quantity = decltype(1.0 / boostunits::coulomb);
inline auto inverse_permittivity_unit_quantity = 1.0 / boostunits::coulomb;
using radian_quantity = boost::units::quantity<boostunits::plane_angle, double>;

// lazy
using metre_quantity = boost::units::quantity<boostunits::length, double>;
using volt_quantity = boost::units::quantity<boostunits::electric_potential, double>;
namespace experimental {
constexpr auto V = boostunits::volt;
constexpr auto N = boostunits::newton;
constexpr auto C = boostunits::coulomb;
constexpr auto m = boostunits::metre;
constexpr auto mm = millimetre;
}  // namespace experimental
// end lazy
// experimental
using dots_quantity = dimensionless_quantity;
// end experimental
}  // namespace LOEF
#endif
