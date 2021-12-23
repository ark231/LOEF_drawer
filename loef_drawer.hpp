#ifndef LOEF_DRAWER_HPP
#define LOEF_DRAWER_HPP

#include <QImage>
#include <QJsonObject>
#include <QKeyEvent>
#include <QWidget>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "charges.hpp"
#include "general_consts.hpp"
#include "vec2d.hpp"
namespace LOEF {
class state_charge_selected_;
class id_handler;
}  // namespace LOEF
// using LOEF::id_type = size_t;

// lazy
namespace LOEF {
namespace experimental {
class electric_potential;
}
}  // namespace LOEF
// end lazy

class LOEF_drawer : public QWidget {
    Q_OBJECT
   public:
    explicit LOEF_drawer(QWidget *parent = nullptr);
    LOEF::id_type create_fixed_charge(LOEF::coulomb_quantity initial_charge = LOEF::initial_fixed_charge,
                                      LOEF::millimetre_quantity initial_X = LOEF::initial_fixed_pos_x,
                                      LOEF::millimetre_quantity initial_Y = LOEF::initial_fixed_pos_y);
    void destroy_all_fixed_charges();
    std::tuple<LOEF::coulomb_quantity, LOEF::millimetre_quantity, LOEF::millimetre_quantity> get_fixed_charge_info(
        LOEF::id_type);
    void request_draw_LOEF(bool yes_or_no);
    QJsonObject create_save_data();
    void select_fixed_charge(LOEF::id_type);
    void unselect_fixed_charge(LOEF::id_type);
    void unselect_all_selected_fixed_charge();

    // lazy impl
    void set_electric_potential(LOEF::experimental::electric_potential *of_parent);
    std::vector<LOEF::fixed_charge> get_fixed_charges();
    // end lazy impl

   private:
    LOEF::dot_per_millimetre_quantity dpmm_;
    std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges_;
    LOEF::id_handler *fixed_charge_id_handler_;
    std::unordered_map<LOEF::id_type, LOEF::charge_pen> charge_pens_;
    std::unordered_map<LOEF::id_type, std::shared_ptr<LOEF::LOEF_path>> charge_paths_;
    LOEF::id_handler *charge_pen_id_handler_;
    LOEF::state_charge_selected_ *charge_selected_manually_;
    LOEF::state_charge_selected_ *charge_selected_automatically_;
    LOEF::inverse_permittivity_quantity inverse_permittivity_ = LOEF::initial_inverse_permittivity;
    bool draw_LOEF_requested = true;
    bool is_multi_selecting = false;

    // lazy impl
    LOEF::experimental::electric_potential *electric_potential_handler = nullptr;
    // end lazy impl

    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

    void calc_LOEF_from_fixed_charges(decltype(fixed_charges_) &, int width, int height);
    void prepare_LOEF_pathes();
    void replace_fixed_charge(const LOEF::id_type id, const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                              const std::optional<LOEF::vec2d> &maybe_new_pos);
    void replace_fixed_charge(const LOEF::id_type id, const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                              const LOEF::millimetre_quantity new_pos_x, const LOEF::millimetre_quantity new_pos_y);
    void clear_and_redraw();

    // lazy impl
    QImage prepare_electric_potential_image();
    // end lazy impl

   signals:
    void fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity new_X,
                                       LOEF::millimetre_quantity new_Y);
    void fixed_charge_selected(LOEF::id_type id);

   private slots:
    void slot_fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity X, LOEF::millimetre_quantity Y);
    void slot_fixed_charge_charge_changed(LOEF::id_type id, LOEF::coulomb_quantity);
    void slot_fixed_charge_destroyed(const LOEF::id_type id);
    void slot_inverse_permittivity_changed(double new_value);
};

#endif  // LOEF_DRAWER_HPP
