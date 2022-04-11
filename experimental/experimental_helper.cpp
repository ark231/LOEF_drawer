#include "experimental_helper.hpp"

#include <cmath>
#include <stdexcept>
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
signed int get_sign(double num) {
    if (qFuzzyIsNull(num)) {
        return 0;
    }
    return 1 ? num > 0 : -1;
}
enum class angle { vertical, horizontal, other };  // NOTE: ほんとに「角度」なのか怪しいが、、
angle angle_of(vec2d vec) {
    if (qFuzzyIsNull(vec.length() / mm)) {
        throw std::invalid_argument("angle of zero vector is not defined");
    } else if (qFuzzyIsNull(vec.x() / mm)) {
        return angle::vertical;
    } else if (qFuzzyIsNull(vec.y() / mm)) {
        return angle::horizontal;
    } else {
        return angle::other;
    }
}
bool are_on_a_line(std::initializer_list<vec2d> points) {
    if (points.size() <= 2) {
        // 1点、2点は同一直線上に必ずある(0点は今のところ含まれてる)
        return true;
    }
    auto points_ptr = points.begin();
    vec2d direction = points_ptr[1] - points_ptr[0];
    bool is_direction_already_found = not qFuzzyIsNull(direction.length().value());
    for (size_t i = 2; i < points.size(); i++) {
        vec2d base_to_point = points_ptr[i] - points_ptr[0];
        if (qFuzzyIsNull(base_to_point.length().value())) {
            continue;
        } else if (not is_direction_already_found) {
            direction = base_to_point;
            is_direction_already_found = true;
            continue;
        }
        switch (angle_of(base_to_point)) {
            case angle::horizontal:
                if (not qFuzzyIsNull(base_to_point.y() / mm)) {
                    //一個でもずれてたら、同一直線上でない&&水平線上ならy成分は0のはず(xは任意)
                    return false;
                }
                break;
            case angle::vertical:
                if (not qFuzzyIsNull(base_to_point.x() / mm)) {
                    //一個でもずれてたら、同一直線上でない
                    return false;
                }
                break;
            default:
                if (not qFuzzyCompare(base_to_point.x() / direction.x(), base_to_point.y() / direction.y())) {
                    //一個でもずれてたら、同一直線上でない
                    return false;
                }
                break;
        }
    }
    return true;
}
bool is_in_between(vec2d min_edge, vec2d value, vec2d max_edge) {
    // qDebug() << min_edge << value << max_edge;
    vec2d val_to_min = min_edge - value, val_to_max = max_edge - value;
    Q_ASSERT(are_on_a_line({min_edge, value, max_edge}));
    if (qFuzzyIsNull(val_to_min.length() / mm) || qFuzzyIsNull(val_to_max.length() / mm)) {
        return true;  //端点を含む
    }
    if (qFuzzyIsNull(val_to_min.x() / mm)) {
        return val_to_min.y() / val_to_max.y() < 0.0;  //間にあったら両端点までのベクトルは逆向きになるはず
    } else {
        // NOTE: 上で、同じ点は除外した
        return val_to_min.x() / val_to_max.x() < 0.0;  //間にあったら両端点までのベクトルは逆向きになるはず
    }
}
}  // namespace experimental
}  // namespace LOEF
