#ifndef LOEF_DRAWER_CHARGE_PEN
#define LOEF_DRAWER_CHARGE_PEN
#include <QPainterPath>
#include <iterator>
#include <memory>
#include <utility>

#include "basic_charge.hpp"
#include "general_consts.hpp"
#include "vec2d.hpp"
// lazy
#include "charges.hpp"
// end lazy
namespace LOEF {
class LOEF_path : public QPainterPath {
    using QPainterPath::QPainterPath;
    bool is_positive_ = true;

   public:
    void set_is_positive(bool) noexcept;
    bool is_positive() const noexcept;
};
enum class step_status { CONTINUE, FINISH, ABORT };
class charge_pen {
    bool is_positive_ = true;
    vec2d position_;
    vec2d previous_delta_position_;
    millimetre_quantity interval_;
    int max_x = 0;
    int max_y = 0;
    std::shared_ptr<LOEF_path> path;
    // lazy
   public:
    std::pair<id_type, fixed_charge> origin;
    // end lazy

   public:
    charge_pen(bool is_positive, vec2d initial_position, millimetre_quantity interval, int max_x, int max_y,
               dot_per_millimetre_quantity dpmm);
    charge_pen() {}
    template <class fixed_charge_map_iterator_>
    step_status step_forward(fixed_charge_map_iterator_ begin, fixed_charge_map_iterator_ end,
                             dot_per_millimetre_quantity dpmm);
    std::shared_ptr<LOEF_path> get_path();
    // lazy
    vec2d position();
    bool is_on_screen(dot_per_millimetre_quantity);
    // end lazy
};
}  // namespace LOEF

#ifndef LOEF_DRAWER_CHARGES_LIBRARY_BUILD
#    include "charge_pen.cpp"  //implementation of template function
#endif

#endif
