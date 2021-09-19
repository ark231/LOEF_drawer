#ifndef LOEF_DRAWER_CHARGE_PEN
#define LOEF_DRAWER_CHARGE_PEN
#include <QPainterPath>
#include <iterator>
#include <memory>

#include "basic_charge.hpp"
#include "general_consts.hpp"
#include "vec2d.hpp"
namespace LOEF {
using screen_mm = double;
class LOEF_path : public QPainterPath {
    using QPainterPath::QPainterPath;
};
class charge_pen {
    bool is_positive_ = true;
    vec2d position_;
    screen_mm interval_ = 1;
    int max_x = 0;
    int max_y = 0;
    std::shared_ptr<LOEF_path> path;

   public:
    charge_pen(bool is_positive, vec2d initial_position, screen_mm interval, int max_x, int max_y);
    charge_pen() {}
    template <class fixed_charge_map_iterator_>
    bool step_forward(fixed_charge_map_iterator_ begin, fixed_charge_map_iterator_ end, double dpmm);
    std::shared_ptr<LOEF_path> get_path();
};
}  // namespace LOEF

#ifndef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
#include "charge_pen.cpp"  //implementation of template function
#endif

#endif
