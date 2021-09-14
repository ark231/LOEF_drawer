#include "charge_pen.hpp"
namespace LOEF {
charge_pen::charge_pen(bool is_positive, vec2d initial_position, screen_mm interval, int max_x, int max_y)
    : is_positive_(is_positive),
      position_(initial_position),
      interval_(interval),
      max_x(max_x),
      max_y(max_y),
      path(new LOEF_path) {
    path->moveTo(static_cast<QPoint>(initial_position));
}

std::shared_ptr<LOEF_path> charge_pen::get_path() { return path; }
}  // namespace LOEF
