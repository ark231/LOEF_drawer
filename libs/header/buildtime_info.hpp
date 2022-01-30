#ifndef LOEF_DRAWER_BUILDTIME_INFO
#define LOEF_DRAWER_BUILDTIME_INFO
#if __has_include("buildtime_info_impl.hpp")
#    include "buildtime_info_impl.hpp"
#else
namespace LOEF {
constexpr const int version_major = -1;
constexpr const int version_minor = -1;
constexpr const int version_patch = -1;
}  // namespace LOEF
#endif
#endif
