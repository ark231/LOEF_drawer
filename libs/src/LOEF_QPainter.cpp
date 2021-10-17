#include "LOEF_QPainter.hpp"

#include <QDebug>
#include <QPen>
#include <QPolygon>
#include <cassert>
#include <cmath>

#include "general_consts.hpp"
#include "math.hpp"
#include "vec2d.hpp"
namespace LOEF {
using dots = double;  //! in order to preserve fraction for multipulication
void painter::set_resolution(dot_per_millimetre_quantity dpmm) { this->dpmm_ = dpmm; }
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
void painter::draw_LOEF_path(const LOEF_path &path) {
    QPainter::save();
    QPainter::drawPath(path);
    auto num_element = path.elementCount();
    if (num_element >= 4) {
        //真ん中での平均の接線
        auto arrow_start_element = path.elementAt((num_element / 2) - 2);
        auto arrow_end_element = path.elementAt(num_element / 2);
        if (!path.is_positive()) {
            std::swap(arrow_start_element, arrow_end_element);
        }
        vec2d vec_arrow((arrow_end_element.x - arrow_start_element.x) / dpmm_,
                        (arrow_end_element.y - arrow_start_element.y) / dpmm_);
        vec_arrow = normalize(vec_arrow) * ARROW_HEIGHT.value();
        vec2d vec_arrow_vertical;
        // cf. vec_arrow・vec_arrow_vertical
        if (qFuzzyIsNull(vec_arrow.x().value())) {                           //垂直
            vec_arrow_vertical = vec2d(1.0 * millimetre, 0.0 * millimetre);  //水平
        } else if (qFuzzyIsNull(vec_arrow.y().value())) {                    //水平
            vec_arrow_vertical = vec2d(0.0 * millimetre, 1.0 * millimetre);  //垂直
        } else {
            vec_arrow_vertical =
                normalize(vec2d(vec_arrow.x(), -(vec_arrow.x().value() / vec_arrow.y().value()) * vec_arrow.x()));
        }
        vec_arrow_vertical *ARROW_HEIGHT.value() / std::sqrt(3.0);
        vec2d arrow_origin(QPoint(arrow_start_element.x, arrow_start_element.y), dpmm_);
        arrow_origin -= vec_arrow / 2.0;
        QPolygon arrow_head(3);
        arrow_head.setPoint(0, static_cast<vec2d>(arrow_origin + vec_arrow).to_QPoint(dpmm_));
        arrow_head.setPoint(1, static_cast<vec2d>(arrow_origin + vec_arrow_vertical).to_QPoint(dpmm_));
        arrow_head.setPoint(2, static_cast<vec2d>(arrow_origin - vec_arrow_vertical).to_QPoint(dpmm_));
        QPainter::setBrush(QBrush(Qt::SolidPattern));
        QPainter::drawPolygon(arrow_head);
    }
    QPainter::restore();
}
}  // namespace LOEF
