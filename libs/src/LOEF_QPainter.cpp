#include "LOEF_QPainter.hpp"

#include <QPen>
#include <cassert>
#include <cmath>

#include "general_consts.hpp"
namespace LOEF {
using dots = double;  //! in order to preserve fraction for multipulication
void painter::set_resolution(dot_per_millimetre_value dpmm) { this->dpmm_ = dpmm; }
void painter::draw_fixed_charge(const fixed_charge &charge) {
    QPainter::save();
    double radius = (radius::FIXED * dpmm_);
    auto position = charge.position();
    QPen pen;
    pen.setWidth(3);
    if (charge.quantity() / boostunits::coulomb > 0) {
        pen.setColor(Qt::red);
    } else if (charge.quantity() / boostunits::coulomb == 0) {
        pen.setColor(Qt::green);
    } else {
        pen.setColor(Qt::black);
    }
    QPainter::setPen(pen);
    QPainter::drawEllipse(position.x() * dpmm_ - radius, position.y() * dpmm_ - radius, 2 * radius,
                          2 * radius);  //! circle is special ellipse
    if (charge.quantity() / boostunits::coulomb > 0) {
        QPainter::drawLine(position.x() * dpmm_ - radius, position.y() * dpmm_, position.x() * dpmm_ + radius,
                           position.y() * dpmm_);
        QPainter::drawLine(position.x() * dpmm_, position.y() * dpmm_ - radius, position.x() * dpmm_,
                           position.y() * dpmm_ + radius);
    } else if (charge.quantity() / boostunits::coulomb == 0) {
        constexpr double scale = 0.7;
        auto x_left = position.x() * dpmm_ - scale * radius / std::sqrt(2);
        auto x_right = position.x() * dpmm_ + scale * radius / std::sqrt(2);
        auto y_top = position.y() * dpmm_ - scale * radius / std::sqrt(2);
        auto y_bottom = position.y() * dpmm_ + scale * radius / std::sqrt(2);
        QPainter::drawLine(x_left, y_bottom, x_left, y_top);
        QPainter::drawLine(x_left, y_top, x_right, y_bottom);
        QPainter::drawLine(x_right, y_bottom, x_right, y_top);
    } else {
        QPainter::drawLine(position.x() * dpmm_ - radius, position.y() * dpmm_, position.x() * dpmm_ + radius,
                           position.y() * dpmm_);
    }
    QPainter::restore();
}
void painter::draw_LOEF_path(const LOEF_path &path) { QPainter::drawPath(path); }
}  // namespace LOEF
