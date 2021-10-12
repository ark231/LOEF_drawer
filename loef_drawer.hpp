#ifndef LOEF_DRAWER_HPP
#define LOEF_DRAWER_HPP

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
class LOEF_drawer : public QWidget {
    Q_OBJECT
   public:
    explicit LOEF_drawer(QWidget *parent = nullptr);
    LOEF::id_type create_fixed_charge(LOEF::coulomb_quantity initial_charge = LOEF::initial_fixed_charge,
                                      LOEF::millimetre_quantity initial_X = LOEF::initial_fixed_pos_x,
                                      LOEF::millimetre_quantity initial_Y = LOEF::initial_fixed_pos_y);
    std::tuple<LOEF::coulomb_quantity, LOEF::millimetre_quantity, LOEF::millimetre_quantity> get_fixed_charge_info(
        LOEF::id_type);

   private:
    LOEF::dot_per_millimetre_quantity dpmm_;
    std::unordered_map<LOEF::id_type, LOEF::fixed_charge> fixed_charges_;
    LOEF::id_handler *fixed_charge_id_handler_;
    std::unordered_map<LOEF::id_type, LOEF::charge_pen> charge_pens_;
    std::unordered_map<LOEF::id_type, std::shared_ptr<LOEF::LOEF_path>> charge_paths_;
    LOEF::id_handler *charge_pen_id_handler_;
    LOEF::state_charge_selected_ *charge_selected_;

    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

    void prepare_LOEF_pathes();
    void replace_fixed_charge(const LOEF::id_type id, const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                              const std::optional<LOEF::vec2d> &maybe_new_pos);
    void replace_fixed_charge(const LOEF::id_type id, const std::optional<LOEF::coulomb_quantity> &maybe_new_charge,
                              const LOEF::millimetre_quantity new_pos_x, const LOEF::millimetre_quantity new_pos_y);
    void clear_and_redraw();

   signals:
    void fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity new_X,
                                       LOEF::millimetre_quantity new_Y);
    void fixed_charge_selected(LOEF::id_type id);

   private slots:
    void slot_fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity X, LOEF::millimetre_quantity Y);
    void slot_fixed_charge_charge_changed(LOEF::id_type id, LOEF::coulomb_quantity);
    void slot_fixed_charge_destroyed(const LOEF::id_type id);
};

#endif  // LOEF_DRAWER_HPP
