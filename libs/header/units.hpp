#ifndef LOEF_DRAWER_UNITS
#define LOEF_DRAWER_UNITS
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
namespace LOEF {
namespace boostunits = boost::units::si;
using millimetre_unit =
    boost::units::make_scaled_unit<boostunits::length,
                                   boost::units::scale<10, boost::units::static_rational<-3>>>::type;
inline millimetre_unit millimetre;
using millimetre_value = boost::units::quantity<millimetre_unit, double>;
using dot_per_millimetre_unit =
    boost::units::power_typeof_helper<millimetre_unit, boost::units::static_rational<-1>>::type;
inline dot_per_millimetre_unit dot_per_millimetre;
using dot_per_millimetre_value = boost::units::quantity<dot_per_millimetre_unit, double>;
using coulomb_value = boost::units::quantity<boost::units::si::electric_charge, double>;
}  // namespace LOEF
#endif
