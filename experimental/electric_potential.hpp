#ifndef LOEF_DRAWER_ELECTRIC_POTENTIAL
#define LOEF_DRAWER_ELECTRIC_POTENTIAL
#include <vector>

#include "units.hpp"

//適当な実装
namespace LOEF {
namespace experimental {
struct electric_potential {
    bool color_enabled = false;
    bool color_use_input = false;
    bool surface_enabled = false;
    bool disable_LOEF = false;
    bool draw_sample_line = false;
    bool draw_sample_rectangle = false;
    volt_quantity distance = 0.0 * boostunits::volt;
    millimetre_quantity output_sample_diff = 1.0 * mm;

    volt_quantity get_current_max_abs_positive();
    volt_quantity get_current_max_abs_negative();
    void set_current_max_abs_positive(volt_quantity new_value);
    void set_current_max_abs_negative(volt_quantity new_value);
    void hint_add_charge(coulomb_quantity new_charge);
    void hint_delete_charge(coulomb_quantity deleted_charge);

   private:
    volt_quantity current_max_abs_positive = 0.0 * boostunits::volt;
    volt_quantity current_max_abs_negative = 0.0 * boostunits::volt;
};
}  // namespace experimental
}  // namespace LOEF
//適当な実装終わり
#endif
