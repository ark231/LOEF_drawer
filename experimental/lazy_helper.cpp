#include "lazy_helper.hpp"

#include <cmath>
namespace LOEF {
namespace experimental {
QWidget* search_for_origin(QWidget* decendant) {
    auto current = decendant;
    auto parent = decendant->parentWidget();
    while (parent != nullptr) {
        current = parent;
        parent = current->parentWidget();
    }
    return current;
}
bool is_about_same(double v1, double v2, double max_error) { return std::abs(v1 - v2) <= max_error; }
bool is_about_same_rel(double v1, double v2, double max_error) {
    if (v1 == 0.0 && v2 == 0.0) {
        return true;  // v1 == v2 == 0.0
    }
    double max_error_abs = 0.0;
    if (v1 == 0.0) {
        max_error_abs = v2 * max_error;
    } else if (v2 == 0.0) {
        max_error_abs = v1 * max_error;
    } else {
        max_error_abs = std::max(v1, v2) * max_error;
    }
    return std::abs(v1 - v2) <= max_error_abs;
}
}  // namespace experimental
}  // namespace LOEF
