#ifndef LOEF_DRAWER_LAZY_HELPER
#define LOEF_DRAWER_LAZY_HELPER
#include <QWidget>
namespace LOEF {
namespace experimental {
QWidget* search_for_origin(QWidget* decendant);
bool is_about_same(double v1, double v2, double max_error);
bool is_about_same_rel(double v1, double v2, double max_error);
}  // namespace experimental
}  // namespace LOEF
#endif
