#include "lazy_helper.hpp"
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
}  // namespace experimental
}  // namespace LOEF
