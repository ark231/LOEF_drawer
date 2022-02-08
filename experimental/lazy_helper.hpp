#ifndef LOEF_DRAWER_LAZY_HELPER
#define LOEF_DRAWER_LAZY_HELPER
#include <QWidget>
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
            normalize(charge_to_pos).to_dimentionless() * (fixed_charge.quantity() / (charge_to_pos.length_square()));
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
        auto 候補 = (itr->second.position() - pos).length();
        if (候補 < current_least) {
            current_least = 候補;
        }
    }
    return current_least;
}
template <class MapIterator>
id_type find_closest(MapIterator fixed_begin, MapIterator fixed_end, vec2d pos,
                     std::function<bool(fixed_charge)> exclude_cond) {
    millimetre_quantity 最短距離 = std::numeric_limits<millimetre_quantity::value_type>::infinity() * mm;
    id_type 最短id = -1;
    for (auto itr = fixed_begin; itr != fixed_end; itr++) {
        if (exclude_cond(itr->second)) {
            continue;
        }
        auto 候補 = (itr->second.position() - pos).length();
        if (候補 < 最短距離) {
            最短距離 = 候補;
            最短id = itr->first;
        }
    }
    return 最短id;
}
}  // namespace experimental
}  // namespace LOEF
#endif
