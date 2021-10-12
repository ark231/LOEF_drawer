#include "fixed_charge.hpp"

#include <QDebug>
#include <QtGlobal>
#include <cmath>
#include <utility>

#include "general_consts.hpp"
#include "math.hpp"
namespace LOEF {
template <class fixed_charge_containter_itr>
std::vector<vec2d> fixed_charge::calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                                   int num_result, int num_sample) const {
    std::vector<vec2d> result;
    result.reserve(num_result);
    std::vector<std::pair<vec2d, electric_field_strength_quantity>> samples;
    samples.reserve(num_sample);
    /*
    std::vector<electric_field_strength_quantity> sample_electric_field_strengths;  //μN/C
    sample_electric_field_strengths.reserve(num_sample);
    std::vector<vec2d> sample_positions;
    sample_positions.reserve(num_sample);
    */
    decltype(1.0 * electric_field_strength_unit_quantity) sum_electric_field_strength;
    millimetre_quantity radius = radius::FIXED + 0.1 * millimetre;
    electric_field_strength_quantity current_max_strength = -1.0 * electric_field_strength_unit_quantity;
    size_t index_current_max = -1;
    for (auto i = 0; i < num_sample; i++) {
        auto current_argument = (static_cast<double>(i) / num_sample) * 2 * pi * boostunits::radian;
        electric_field sample_electric_field = {0.0 * (electric_field_strength_unit_quantity),
                                                0.0 * (electric_field_strength_unit_quantity)};
        vec2d sample_position =
            this->position_ +
            (radius / millimetre) * vec2d(std::cos(current_argument / boostunits::radian) * millimetre,
                                          std::sin(current_argument / boostunits::radian) * millimetre);
        // sample_positions.push_back(sample_position);
        for (auto fixed_charge = begin; fixed_charge != end; fixed_charge++) {
            vec2d charge_to_sample = sample_position - fixed_charge->second.position();
            electric_field_strength_quantity sample_field_strength =
                (fixed_charge->second.quantity() / charge_to_sample.length_square());
            auto field_direction = charge_to_sample.to_dimentionless();
            sample_electric_field += electric_field(field_direction.x() * sample_field_strength,
                                                    field_direction.y() * sample_field_strength);
        }
        // sample_electric_field_strengths.push_back(sample_electric_field.length());
        samples.push_back(std::make_pair(sample_position, sample_electric_field.length()));
        sum_electric_field_strength += sample_electric_field.length();
        if (sample_electric_field.length() > current_max_strength) {
            current_max_strength = sample_electric_field.length();
            index_current_max = i;
        }
    }
    const decltype(current_max_strength) max_strength = current_max_strength;
    const decltype(index_current_max) max_index = index_current_max;
    Q_ASSERT_X(max_strength >= 0.0 * electric_field_strength_unit_quantity, "calc_pen_init_pos",
               "not found strongest point");
    electric_field_strength_quantity current_sum = 0.0 * electric_field_strength_unit_quantity;
    result.push_back(samples[max_index].first);  //起点は必ず入る
    /*添字をmax_index分ずらして参照する*/
    for (auto j = max_index; j != (num_sample - 1 + max_index) % num_sample; j = (j + 1) % num_sample) {
        if (current_sum > sum_electric_field_strength / static_cast<double>(num_result)) {
            result.push_back(samples[j].first);
            current_sum = 0.0 * electric_field_strength_unit_quantity;
        }
        current_sum += samples[j].second;
    }
    return result;
}
template <class fixed_charge_containter_itr>
void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end) {
    for (auto fixed_itr = begin; fixed_itr != end; fixed_itr++) {
        fixed_itr->second.offsets_enterd_pen_.clear();
    }
}
#ifdef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
bool fixed_charge::pen_arrive(vec2d offset, decltype(1.0 / boostunits::coulomb) inverse_permittivity) {
    // qDebug() << "@fixed_charge " << __func__ << "charge pen arrives num_offset:" << offsets_enterd_pen_.size();
    if (this->needs_pens(inverse_permittivity)) {
        offsets_enterd_pen_.push_back(offset);
        return true;
    } else {
        return false;
    }
}
bool fixed_charge::needs_pens(decltype(1.0 / boostunits::coulomb) inverse_permittivity) {
    return offsets_enterd_pen_.size() < std::abs(this->quantity_ * inverse_permittivity);
}
#endif
}  // namespace LOEF
