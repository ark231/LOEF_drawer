#include "fixed_charge.hpp"

#include <QDebug>

#include "general_consts.hpp"
#include "math.hpp"
namespace LOEF {
template <class fixed_charge_containter_itr>
std::vector<vec2d> fixed_charge::calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                                   int num_result, int num_sample) const {
    std::vector<vec2d> result;
    result.reserve(num_result);
    std::vector<electric_field_strength_quantity> sample_electric_field_strengths;  //μN/C
    sample_electric_field_strengths.reserve(num_sample);
    std::vector<vec2d> sample_positions;
    sample_positions.reserve(num_sample);
    decltype(1.0 * electric_field_strength_unit_quantity) sum_electric_field_strength;
    millimetre_quantity radius = radius::FIXED + 0.1 * millimetre;
    for (auto i = 0; i < num_sample; i++) {
        auto current_argument = (static_cast<double>(i) / num_sample) * 2 * pi * boostunits::radian;
        electric_field sample_electric_field = {0.0 * (electric_field_strength_unit_quantity),
                                                0.0 * (electric_field_strength_unit_quantity)};
        vec2d sample_position =
            this->position_ +
            (radius / millimetre) * vec2d(std::cos(current_argument / boostunits::radian) * millimetre,
                                          std::sin(current_argument / boostunits::radian) * millimetre);
        sample_positions.push_back(sample_position);
        for (auto fixed_charge = begin; fixed_charge != end; fixed_charge++) {
            vec2d charge_to_sample = sample_position - fixed_charge->second.position();
            electric_field_strength_quantity sample_field_strength =
                (fixed_charge->second.quantity() / charge_to_sample.length_square());
            auto field_direction = charge_to_sample.to_dimentionless();
            sample_electric_field += electric_field(field_direction.x() * sample_field_strength,
                                                    field_direction.y() * sample_field_strength);
        }
        sample_electric_field_strengths.push_back(sample_electric_field.length());
        sum_electric_field_strength += sample_electric_field.length();
    }
    electric_field_strength_quantity current_sum = 0.0 * electric_field_strength_unit_quantity;
    result.push_back(sample_positions[0]);  //起点は必ず入る
    for (auto j = 0; j < num_sample; j++) {
        if (current_sum > sum_electric_field_strength / static_cast<double>(num_result)) {
            result.push_back(sample_positions[j]);
            current_sum = 0.0 * electric_field_strength_unit_quantity;
        }
        current_sum += sample_electric_field_strengths[j];
    }
    return result;
}
}  // namespace LOEF
