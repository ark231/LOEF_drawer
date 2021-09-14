#include "LOEF_QPainter.hpp"

#include <QPen>
#include <cassert>
#include <cmath>

#include "general_consts.hpp"
namespace LOEF {
using dots = double;  //! in order to preserve fraction for multipulication
void painter::set_resolution(double dpmm) { this->dpmm_ = dpmm; }
void painter::draw_fixed_charge(const fixed_charge &charge) {
    QPainter::save();
    assert(this->dpmm_ > 0);
    double radius = radius::FIXED * dpmm_;
    auto position = charge.position();
    QPen pen;
    pen.setWidth(3);
    if (charge.quantity() > 0) {
        pen.setColor(Qt::red);
    } else if (charge.quantity() == 0) {
        pen.setColor(Qt::green);
    } else {
        pen.setColor(Qt::black);
    }
    QPainter::setPen(pen);
    QPainter::drawEllipse(position.x() - radius, position.y() - radius, 2 * radius,
                          2 * radius);  //! circle is special ellipse
    if (charge.quantity() > 0) {
        QPainter::drawLine(position.x() - radius, position.y(), position.x() + radius, position.y());
        QPainter::drawLine(position.x(), position.y() - radius, position.x(), position.y() + radius);
    } else if (charge.quantity() == 0) {
        constexpr double scale = 0.7;
        auto x_left = position.x() - scale * radius / std::sqrt(2);
        auto x_right = position.x() + scale * radius / std::sqrt(2);
        auto y_top = position.y() - scale * radius / std::sqrt(2);
        auto y_bottom = position.y() + scale * radius / std::sqrt(2);
        QPainter::drawLine(x_left, y_bottom, x_left, y_top);
        QPainter::drawLine(x_left, y_top, x_right, y_bottom);
        QPainter::drawLine(x_right, y_bottom, x_right, y_top);
    } else {
        QPainter::drawLine(position.x() - radius, position.y(), position.x() + radius, position.y());
    }
    QPainter::restore();
}
void painter::draw_LOEF_path(const LOEF_path &path) { QPainter::drawPath(path); }
}  // namespace LOEF
