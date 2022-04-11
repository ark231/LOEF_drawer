#ifndef LOEF_DRAWER_EXPERIMENTAL_HELPER
#define LOEF_DRAWER_EXPERIMENTAL_HELPER
#include <QWidget>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

// lazy
#include <functional>
#include <numeric>

#include "charges.hpp"
// end lazy
#include "units.hpp"
#include "vec2d.hpp"
namespace LOEF {
namespace experimental {
QWidget* search_for_origin(QWidget* decendant);
bool is_about_same(double v1, double v2, double max_error);
bool is_about_same_rel(double v1, double v2, double max_error);
template <class MapIterator>
electric_field calc_electric_field(vec2d pos, MapIterator fixed_begin, MapIterator fixed_end) {
    LOEF::electric_field electric_field(0, 0);
    for (auto fixed_charge_itr = fixed_begin; fixed_charge_itr != fixed_end; fixed_charge_itr++) {
        auto fixed_charge = fixed_charge_itr->second;
        if (fixed_charge.quantity() == 0.0 * C) {
            continue;  //非荷電電荷のど真ん中をすり抜けても0除算しないようにするため
        }
        vec2d charge_to_pos = pos - fixed_charge.position();
        electric_field +=
            normalize(charge_to_pos).to_dimensionless() * (fixed_charge.quantity() / (charge_to_pos.length_square()));
    }
    return electric_field;
}
template <class T, class Iterator>
bool contains(
    Iterator begin, Iterator end, T value, std::function<T(Iterator&)> retriver = [](Iterator& itr) { return *itr; }) {
    for (auto itr = begin; itr != end; itr++) {
        if (retriver(itr) == value) {
            return true;
        }
    }
    return false;
}
template <class MapIterator>
millimetre_quantity calc_shortest_distance(
    MapIterator fixed_begin, MapIterator fixed_end, vec2d pos, std::vector<id_type> excludes,
    std::function<bool(fixed_charge)> exclude_cond = [](fixed_charge) { return false; }) {
    millimetre_quantity current_least = std::numeric_limits<millimetre_quantity::value_type>::infinity() * mm;
    for (auto itr = fixed_begin; itr != fixed_end; itr++) {
        if (contains<>(excludes.begin(), excludes.end(), itr->first) || exclude_cond(itr->second)) {
            continue;
        }
        auto new_value = (itr->second.position() - pos).length();
        if (new_value < current_least) {
            current_least = new_value;
        }
    }
    return current_least;
}
template <class MapIterator>
id_type find_closest(
    MapIterator fixed_begin, MapIterator fixed_end, vec2d pos,
    std::function<bool(fixed_charge)> exclude_cond = [](fixed_charge) { return false; }) {
    millimetre_quantity shortest_distance = std::numeric_limits<millimetre_quantity::value_type>::infinity() * mm;
    id_type id_shortest = -1;
    for (auto itr = fixed_begin; itr != fixed_end; itr++) {
        if (exclude_cond(itr->second)) {
            continue;
        }
        auto new_value = (itr->second.position() - pos).length();
        if (new_value < shortest_distance) {
            shortest_distance = new_value;
            id_shortest = itr->first;
        }
    }
    return id_shortest;
}
/**
 * @brief 関数f(x)を線形補間して、所定の出力となる入力を返す.
 * \ff(x_0)=y_0\f,\ff(x_1)=y_1\fの時に、\ff(x)\fを線形補間して\ff^{-1}(y_2)\fを求める
 *
 * @tparam QUANTITY_X 入力の物理単位
 * @tparam QUANTITY_Y 出力の物理単位
 * @param x0
 * @param x1
 * @param y0
 * @param y1
 * @param y2 目標の出力
 * @return QUANTITY_X y2となるxの値
 */
template <class QUANTITY_X, class QUANTITY_Y>
QUANTITY_X inverse_linear_interpolation(QUANTITY_X x0, QUANTITY_Y y0, QUANTITY_X x1, QUANTITY_Y y1, QUANTITY_Y y2) {
    auto slope = (y1 - y0) / (x1 - x0);
    return x0 + (y2 - y0) / slope;
}
//ベクトル向けの部分的特殊化
template <class QUANTITY_Y>
vec2d inverse_linear_interpolation(vec2d x0, QUANTITY_Y y0, vec2d x1, QUANTITY_Y y1, QUANTITY_Y y2) {
    auto slope = (y1 - y0) / ((x1 - x0).length());
    auto direction = normalize(x1 - x0).to_dimensionless();
    return x0 + ((y2 - y0) / slope) * direction;
}
/**
 * @brief
 *
 * @tparam QUANTITY_X
 * @tparam QUANTITY_Y
 * @param p0
 * @param p1
 * @param y2
 * @return QUANTITY_X
 */
template <class QUANTITY_X, class QUANTITY_Y>
QUANTITY_X inverse_linear_interpolation(std::pair<QUANTITY_X, QUANTITY_Y> p0, std::pair<QUANTITY_X, QUANTITY_Y> p1,
                                        QUANTITY_Y y2) {
    return inverse_linear_interpolation(p0.first, p0.second, p1.first, p1.second, y2);
}
bool are_on_a_line(std::initializer_list<vec2d> points);
/**
 * @brief 値が指定した閉区間内にあるかを判定する
 *
 * @tparam QUANTITY
 * @param min_edge 区間の最小
 * @param value 判定対象
 * @param max_edge 区間の最大
 * @return true 値は区間内にある
 * @return false 値は区間内にない
 */
template <class QUANTITY>
bool is_in_between(QUANTITY min_edge, QUANTITY value, QUANTITY max_edge) {
    return min_edge <= value && value <= max_edge;
}
bool is_in_between(vec2d min_edge, vec2d value, vec2d max_edge);
template <class MapIterator>
volt_quantity calc_electric_potential(vec2d pos, MapIterator fixed_begin, MapIterator fixed_end) {
    volt_quantity potential = 0.0 * V;
    for (auto fixed_charge_itr = fixed_begin; fixed_charge_itr != fixed_end; fixed_charge_itr++) {
        auto fixed_charge = fixed_charge_itr->second;
        vec2d charge_to_pos = pos - fixed_charge.position();
        if (qFuzzyIsNull(charge_to_pos.length() / mm)) {
            throw std::domain_error(
                "devision by zero: tried to calculate electric potential at exactly where a charge exists");
        }
        potential += static_cast<volt_quantity>(k0 * (fixed_charge.quantity() / (charge_to_pos.length())));
    }
    return potential;
}
signed int get_sign(double num);

template <class QUANTITY>
QUANTITY calc_nearest_multiple(QUANTITY target, QUANTITY unit) {
    // qDebug() << "@calc_nearest_multiple(" << target << unit << ")";
    namespace bu = boost::units;
    double sign = get_sign(target.value());
    QUANTITY target_abs = bu::abs(target);
    QUANTITY unit_abs = bu::abs(unit);
    QUANTITY remainder = bu::fmod(target_abs, unit_abs);
    if (qFuzzyIsNull(remainder.value())) {
        return target_abs;
    }
    if (remainder > unit_abs / 2.0) {
        return sign * (target_abs + (unit_abs - remainder));
    } else {
        return sign * (target_abs - remainder);
    }
}
}  // namespace experimental
}  // namespace LOEF
#endif
