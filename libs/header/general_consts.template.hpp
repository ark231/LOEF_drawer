#ifndef LOEF_DRAWER_GENERAL_CONSTS
#define LOEF_DRAWER_GENERAL_CONSTS
#include "units.hpp"
namespace LOEF {
constexpr const char* application_name = "LOEF_drawer";
constexpr const int version_major = ${LOEF_drawer_VERSION_MAJOR};
constexpr const int version_minor = ${LOEF_drawer_VERSION_MINOR};
constexpr const int version_patch = ${LOEF_drawer_VERSION_PATCH};
using id_type = size_t;

/*GLOBAL VARIABLES!*/  // This is not the best solution,I know, but I don't have enough time
namespace radius {
inline millimetre_quantity FIXED = 2.5 * millimetre;
}  // namespace radius
inline millimetre_quantity ARROW_HEIGHT = 3.0 * millimetre;
inline millimetre_quantity initial_fixed_pos_x = 50 * millimetre;
inline millimetre_quantity initial_fixed_pos_y = 50 * millimetre;
inline coulomb_quantity initial_fixed_charge = 0.0 * boostunits::coulomb;
inline inverse_permittivity_quantity initial_inverse_permittivity = 12.0 * inverse_permittivity_unit_quantity;
inline millimetre_quantity interval_steps = 0.1 * millimetre;
}  // namespace LOEF
#endif
