#include "fixed_charge.hpp"

#include <QDebug>
#include <QtGlobal>
#include <cmath>
#include <limits>
#include <optional>
#include <utility>

#include "debug_outputs.hpp"
#include "general_consts.hpp"
#include "math.hpp"
namespace LOEF {
inline const auto less_argument = [](const vec2d& a, const vec2d& b) { return argument(a) < argument(b); };
template <class fixed_charge_containter_itr>
std::vector<vec2d> fixed_charge::calc_pen_init_pos(fixed_charge_containter_itr begin, fixed_charge_containter_itr end,
                                                   unsigned int num_result, unsigned int num_sample) const {
    if (this->quantity_ < 0.0 * boostunits::coulomb && !this->needs_pens()) {
        return std::vector<vec2d>();
    }
    std::vector<vec2d> result;
    result.reserve(num_result);
    std::vector<std::pair<vec2d, electric_field_strength_quantity>> samples;
    samples.reserve(num_sample);
    decltype(1.0 * electric_field_strength_unit_quantity) sum_electric_field_strength;
    millimetre_quantity radius = radius::FIXED + 0.1 * millimetre;
    electric_field_strength_quantity current_max_strength = -1.0 * electric_field_strength_unit_quantity;
    std::optional<size_t> index_current_max;
    /*サンプル生成*/
    for (size_t i = 0; i < num_sample; i++) {
        auto current_argument = (static_cast<double>(i) / num_sample) * 2 * pi * boostunits::radian;
        if (this->quantity_ < 0.0 * boostunits::coulomb && !this->offsets_entered_pen_->empty()) {
            current_argument += argument(this->offsets_entered_pen_->at(0));
        }
        electric_field sample_electric_field = {0.0 * (electric_field_strength_unit_quantity),
                                                0.0 * (electric_field_strength_unit_quantity)};
        vec2d sample_position = this->position_ + ((radius + 1.0 * millimetre)).value() *
                                                      vec2d(boost::units::cos(current_argument) * millimetre,
                                                            boost::units::sin(current_argument) * millimetre);
        for (auto fixed_charge = begin; fixed_charge != end; fixed_charge++) {
            vec2d charge_to_sample = sample_position - fixed_charge->second.position();
            electric_field_strength_quantity sample_field_strength =
                (fixed_charge->second.quantity() / charge_to_sample.length_square());
            auto field_direction = charge_to_sample.to_dimentionless();
            sample_electric_field += electric_field(field_direction.x() * sample_field_strength,
                                                    field_direction.y() * sample_field_strength);
        }
        samples.push_back(std::make_pair(sample_position, sample_electric_field.length()));
        sum_electric_field_strength += sample_electric_field.length();
        if (sample_electric_field.length() > current_max_strength) {
            current_max_strength = sample_electric_field.length();
            index_current_max = i;
        }
    }
    const decltype(current_max_strength) max_strength = current_max_strength;
    const size_t max_index = index_current_max.value_or(0);
    Q_ASSERT_X(!qFuzzyIsNull(max_strength.value()) && max_strength.value() >= 0.0 && index_current_max,
               "calc_pen_init_pos", "no strongest point found ");
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
    if (this->quantity_ > 0.0 * boostunits::coulomb) {
        return result;
    } else if (this->quantity_ < 0.0 * boostunits::coulomb) {
        std::vector<vec2d>& naive_results = result;  // rename
        if (offsets_entered_pen_->empty()) {
            return naive_results;  //何も考える必要はない
        }
        std::vector<vec2d> naive_offsets;
        naive_offsets.reserve(naive_results.size());
        std::transform(naive_results.begin(), naive_results.end(), std::back_inserter(naive_offsets),
                       [this](const vec2d& pos) { return pos - this->position_; });
        std::vector<vec2d> negative_results;
        negative_results.reserve(num_result);
        auto offsets_entered_pen_tmp = this->offsets_entered_pen_;
        std::sort(offsets_entered_pen_tmp->begin(), offsets_entered_pen_tmp->end(), less_argument);
        std::sort(naive_offsets.begin(), naive_offsets.end(), less_argument);
        std::vector<std::pair<vec2d, radian_quantity>> accuracy_naive_offsets;
        for (const auto& naive_offset : naive_offsets) {
            radian_quantity current_min_diff_arg = std::numeric_limits<double>::infinity() * boostunits::radian;
            for (const auto& offset_entered_pen : *offsets_entered_pen_tmp) {
                auto diff_arg = boost::units::abs(argument(offset_entered_pen) - argument(naive_offset));
                if (diff_arg > pi * boostunits::radian) {
                    diff_arg = 2 * pi * boostunits::radian - diff_arg;
                }
                if (diff_arg < current_min_diff_arg) {
                    current_min_diff_arg = diff_arg;
                }
            }
            accuracy_naive_offsets.push_back(std::make_pair(naive_offset, current_min_diff_arg));
        }
        std::sort(accuracy_naive_offsets.begin(), accuracy_naive_offsets.end(),
                  [](auto a, auto b) { return a.second > b.second; });  //降順
        for (size_t i = 0; i < num_needed_pens(); i++) {
            negative_results.push_back(this->position_ + accuracy_naive_offsets[i].first);
        }
        return negative_results;
    } else {
        return std::vector<vec2d>();
    }
}
template <class fixed_charge_containter_itr>
void clear_pens_arrival_to_fixed_charges(fixed_charge_containter_itr begin, fixed_charge_containter_itr end) {
    for (auto fixed_itr = begin; fixed_itr != end; fixed_itr++) {
        fixed_itr->second.offsets_entered_pen_->clear();
    }
}
#ifdef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
fixed_charge::fixed_charge() : offsets_entered_pen_(new std::vector<vec2d>) {}
fixed_charge::fixed_charge(coulomb_quantity initial_quantity, millimetre_quantity initial_x,
                           millimetre_quantity initial_y)
    : fixed_charge(initial_quantity, vec2d(initial_x, initial_y)) {}
fixed_charge::fixed_charge(coulomb_quantity initial_quantity, vec2d initial_position)
    : offsets_entered_pen_(new std::vector<vec2d>) {
    this->quantity_ = initial_quantity;
    this->position_ = initial_position;
}
bool fixed_charge::pen_arrive(vec2d offset) {
    if (this->needs_pens()) {
        offsets_entered_pen_->push_back(offset);
        return true;
    } else {
        return false;
    }
}
bool fixed_charge::needs_pens() const {
    return offsets_entered_pen_->size() < std::abs(this->quantity_ * inverse_permittivity_);
}
size_t fixed_charge::num_needed_pens() const {
    return std::abs(this->quantity_ * inverse_permittivity_) - offsets_entered_pen_->size();
}
void fixed_charge::update_inverse_permittivity(inverse_permittivity_quantity new_quantity) {
    this->inverse_permittivity_ = new_quantity;
}
#endif
}  // namespace LOEF
