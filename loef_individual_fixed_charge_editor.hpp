#ifndef LOEF_INDIVIDUAL_FIXED_CHARGE_EDITOR_HPP
#define LOEF_INDIVIDUAL_FIXED_CHARGE_EDITOR_HPP

#include <QWidget>

#include "general_consts.hpp"
#include "qt_consts.hpp"
#include "units.hpp"

namespace Ui {
class LOEF_individual_fixed_charge_editor;
}

class LOEF_individual_fixed_charge_editor : public QWidget {
    Q_OBJECT

   public:
    explicit LOEF_individual_fixed_charge_editor(LOEF::id_type id,
                                                 LOEF::coulomb_quantity initial_charge = LOEF::initial_fixed_charge,
                                                 LOEF::millimetre_quantity initial_X = LOEF::initial_fixed_pos_x,
                                                 LOEF::millimetre_quantity initial_Y = LOEF::initial_fixed_pos_y,
                                                 QWidget *parent = nullptr);
    ~LOEF_individual_fixed_charge_editor();
    LOEF::id_type id();

   private:
    Ui::LOEF_individual_fixed_charge_editor *ui;
    LOEF::id_type id_;
    QPoint moved_pos_ = LOEF::invalid_position;
    void closeEvent(QCloseEvent *) override;
    void moveEvent(QMoveEvent *) override;

   signals:
    void fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity X, LOEF::millimetre_quantity Y);
    void fixed_charge_charge_changed(LOEF::id_type id, LOEF::coulomb_quantity C);
    void fixed_charge_destroyed(LOEF::id_type id);
    void editor_fixed_charge_closed(QPoint);

   private slots:
    void slot_fixed_charge_position_changed(LOEF::id_type id, LOEF::millimetre_quantity X, LOEF::millimetre_quantity Y);
    void on_doubleSpinBox_quantity_valueChanged(double arg1);
    void on_doubleSpinBox_pox_x_valueChanged(double arg1);
    void on_doubleSpinBox_pox_y_valueChanged(double arg1);
    void on_remove_button_clicked();
};

#endif  // LOEF_INDIVIDUAL_FIXED_CHARGE_EDITOR_HPP
