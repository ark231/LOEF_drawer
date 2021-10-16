#ifndef LOEF_DRAWER_GENERAL_CONSTS
#define LOEF_DRAWER_GENERAL_CONSTS
#include "units.hpp"
namespace LOEF {
constexpr const char* application_name = "LOEF_drawer";
constexpr const int version_major = ${LOEF_drawer_VERSION_MAJOR};
constexpr const int version_minor = ${LOEF_drawer_VERSION_MINOR};
constexpr const int version_patch = ${LOEF_drawer_VERSION_PATCH};
using id_type = size_t;
namespace radius {
constexpr millimetre_quantity FIXED = 2.5 * millimetre;  /// note: in the future, this will be inline var for config
}  // namespace radius
constexpr millimetre_quantity ARROW_HEIGHT = 3.0 * millimetre;
constexpr millimetre_quantity initial_fixed_pos_x = 50 * millimetre;
constexpr millimetre_quantity initial_fixed_pos_y = 50 * millimetre;
constexpr coulomb_quantity initial_fixed_charge = 0.0 * boostunits::coulomb;
inline const inverse_permittivity_quantity initial_inverse_permittivity = 12.0 * inverse_permittivity_unit_quantity;
}  // namespace LOEF
#endif
