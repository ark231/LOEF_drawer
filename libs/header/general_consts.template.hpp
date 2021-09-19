#ifndef LOEF_DRAWER_GENERAL_CONSTS
#define LOEF_DRAWER_GENERAL_CONSTS
#include "units.hpp"
namespace LOEF {
constexpr const char* application_name = "LOEF_drawer";
constexpr const int version_major = ${LOEF_drawer_VERSION_MAJOR};
constexpr const int version_minor = ${LOEF_drawer_VERSION_MINOR};
constexpr const int version_patch = ${LOEF_drawer_VERSION_PATCH};
namespace radius {
constexpr millimetre_quantity FIXED = 2.5 * millimetre;
}  // namespace radius
}  // namespace LOEF
#endif
