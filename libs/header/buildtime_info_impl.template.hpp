#ifndef LOEF_DRAWER_BUILDTIME_INFO_IMPL
#define LOEF_DRAWER_BUILDTIME_INFO_IMPL
namespace LOEF {
constexpr const int version_major = ${LOEF_drawer_VERSION_MAJOR};
constexpr const int version_minor = ${LOEF_drawer_VERSION_MINOR};
constexpr const int version_patch = ${LOEF_drawer_VERSION_PATCH};
constexpr const char* version_suffix = "${LOEF_drawer_VERSION_SUFFIX}";
constexpr const char* build_type = "${CMAKE_BUILD_TYPE}";
}  // namespace LOEF
#endif
