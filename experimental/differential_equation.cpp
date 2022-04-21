#include "differential_equation.hpp"

#include "boost/numeric/odeint.hpp"
#include "experimental_helper.hpp"
namespace LOEF {
namespace experimental {
differential_equation_handler::total_polarity differential_equation_handler::set_fixed_charges(
    const std::unordered_map<id_type, fixed_charge> &fixed_charges) {
    fixed_begin_ = fixed_charges.cbegin();
    fixed_end_ = fixed_charges.cend();
    update_fixed_charges_total_polarity();
    return this->fixed_charges_total_polarity_;
}
void differential_equation_handler::update_fixed_charges_total_polarity() {
    coulomb_quantity positive_charge_sum = 0.0 * C, negative_charge_sum = 0.0 * C;
    for (auto iter = fixed_begin_; iter != fixed_end_; iter++) {
        auto charge_quantity = iter->second.quantity();
        if (charge_quantity > 0.0 * C) {
            positive_charge_sum += charge_quantity;
        } else if (charge_quantity < 0.0 * C) {
            negative_charge_sum += charge_quantity;
        }
    }
    auto no_positive_charge_exists = qFuzzyIsNull(positive_charge_sum.value());  //極小電荷は無視？
    auto no_negative_charge_exists = qFuzzyIsNull(negative_charge_sum.value());
    if (no_positive_charge_exists && no_negative_charge_exists) {
        fixed_charges_total_polarity_ = total_polarity::equal;
    } else if (no_positive_charge_exists) {
        fixed_charges_total_polarity_ = total_polarity::negative;
    } else if (no_negative_charge_exists) {
        fixed_charges_total_polarity_ = total_polarity::positive;
    } else if (qFuzzyCompare(positive_charge_sum.value(), boost::units::abs(negative_charge_sum).value())) {
        fixed_charges_total_polarity_ = total_polarity::equal;
    } else if (positive_charge_sum > boost::units::abs(negative_charge_sum)) {
        fixed_charges_total_polarity_ = total_polarity::positive;
    } else if (boost::units::abs(negative_charge_sum) > positive_charge_sum) {
        fixed_charges_total_polarity_ = total_polarity::negative;
    }
}
void differential_equation_handler::set_dpmm(dot_per_millimetre_quantity dpmm) { dpmm_ = dpmm; }
void differential_equation_handler::observe(const LOEF_system<fixed_charge_map_const_iter>::state_type &state,
                                            const double) const {
    Q_ASSERT(current_pen.hasLocalData());
    auto pen = current_pen.localData().second;
    auto path = pen.get_path();
    if (LOEF::experimental::calc_shortest_distance(fixed_begin_, fixed_end_, state, {pen.origin.first},
                                                   [](LOEF::fixed_charge charge) {
                                                       return charge.quantity() == 0.0 * LOEF::boostunits::coulomb;
                                                   }) < LOEF::radius::FIXED) {
        auto idx_entered_charge =
            LOEF::experimental::find_closest(fixed_begin_, fixed_end_, state, [&](LOEF::fixed_charge charge) {
                if (path->is_positive()) {
                    return charge.quantity() >= 0.0 * LOEF::boostunits::coulomb;
                } else {
                    return charge.quantity() <= 0.0 * LOEF::boostunits::coulomb;
                }
            });
        throw EnteredChargeInfo{true, idx_entered_charge, LOEF::vec2d(state)};
    }
    if (not boost::units::isfinite(LOEF::vec2d(state).x()) or not boost::units::isfinite(LOEF::vec2d(state).y())) {
        throw std::range_error("illegal calculation happened");
    }
    if (current_is_every_charge_divergent.localData() && not pen.is_on_screen(dpmm_)) {
        throw std::range_error("go out of screen");
    }
    path->lineTo(LOEF::vec2d(state).to_QPointF(dpmm_));
}
EnteredChargeInfo differential_equation_handler::integrate_pen(std::pair<id_type, charge_pen> item) const {
    current_pen.setLocalData(item);
    auto pen = item.second;
    auto path = pen.get_path();
    using LOEF::experimental::mm;
    LOEF::experimental::LOEF_system<fixed_charge_map_const_iter>::state_type state0 = {pen.position().x().value(),
                                                                                       pen.position().y().value()};
    LOEF::experimental::LOEF_system<fixed_charge_map_const_iter> system(fixed_begin_, fixed_end_, path->is_positive());
    path->moveTo(LOEF::vec2d(state0).to_QPointF(dpmm_));
    bool is_every_charge_divergent = false;
    auto origin = pen.origin.second;
    switch (fixed_charges_total_polarity_) {
        case total_polarity::positive:
            if (origin.quantity() > 0.0 * LOEF::boostunits::coulomb) {
                is_every_charge_divergent = true;
            }
            break;
        case total_polarity::negative:
            if (origin.quantity() < 0.0 * LOEF::boostunits::coulomb) {
                is_every_charge_divergent = true;
            }
            break;
        case total_polarity::equal:
        default:
            break;
    }
    current_is_every_charge_divergent.setLocalData(is_every_charge_divergent);
    try {
        using StepperBase = boost::numeric::odeint::runge_kutta_dopri5<
            LOEF::experimental::LOEF_system<fixed_charge_map_const_iter>::state_type>;
        auto observer = create_observer();
        if (LOEF::experimental::integrate::more_precise) {
            boost::numeric::odeint::integrate_const(
                StepperBase(), std::ref(system), state0, LOEF::experimental::integrate::start_time,
                LOEF::experimental::integrate::end_time, LOEF::experimental::integrate::dt, observer);
            //例外が必ず発出されて以下は飛ばされる
        }  //よって、ここに実質elseがあるのと同じ
        auto stepper = boost::numeric::odeint::make_dense_output(0.001, 0.001, StepperBase());
        if (LOEF::experimental::integrate::less_samples) {
            boost::numeric::odeint::integrate(std::ref(system), state0, LOEF::experimental::integrate::start_time,
                                              LOEF::experimental::integrate::end_time,
                                              LOEF::experimental::integrate::dt, observer);
        } else {
            boost::numeric::odeint::integrate_const(
                stepper, std::ref(system), state0, LOEF::experimental::integrate::start_time,
                LOEF::experimental::integrate::end_time, LOEF::experimental::integrate::dt, observer);
        }
    } catch (std::runtime_error &err) {  //握りつぶす
        // qDebug() << pen_itr->first << err.what();
        return EnteredChargeInfo{false, 0, {qQNaN() * mm, qQNaN() * mm}};
    } catch (EnteredChargeInfo info) {
        return info;
    }
}

}  // namespace experimental
}  // namespace LOEF
