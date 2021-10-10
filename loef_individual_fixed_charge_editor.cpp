#include "loef_individual_fixed_charge_editor.hpp"

#include "ui_loef_individual_fixed_charge_editor.h"

LOEF_individual_fixed_charge_editor::LOEF_individual_fixed_charge_editor(LOEF::id_type id, QWidget *parent)
    : QWidget(parent), ui(new Ui::LOEF_individual_fixed_charge_editor) {
    ui->setupUi(this);
    this->id_ = id;
}

LOEF_individual_fixed_charge_editor::~LOEF_individual_fixed_charge_editor() { delete ui; }
LOEF::id_type LOEF_individual_fixed_charge_editor::id() { return this->id_; }
void LOEF_individual_fixed_charge_editor::slot_fixed_charge_position_changed(LOEF::id_type id,
                                                                             LOEF::millimetre_quantity X,
                                                                             LOEF::millimetre_quantity Y) {
    if (id == this->id_) {
        /*update form*/
        ui->doubleSpinBox_pox_x->setValue(X / LOEF::millimetre);
        ui->doubleSpinBox_pox_y->setValue(Y / LOEF::millimetre);
    }
}

void LOEF_individual_fixed_charge_editor::on_doubleSpinBox_quantity_valueChanged(double arg1) {
    emit fixed_charge_charge_changed(this->id_, arg1 * LOEF::boostunits::coulomb);
}

void LOEF_individual_fixed_charge_editor::on_doubleSpinBox_pox_x_valueChanged(double arg1) {
    emit fixed_charge_position_changed(this->id_, arg1 * LOEF::millimetre,
                                       ui->doubleSpinBox_pox_y->value() * LOEF::millimetre);
}

void LOEF_individual_fixed_charge_editor::on_doubleSpinBox_pox_y_valueChanged(double arg1) {
    emit fixed_charge_position_changed(this->id_, ui->doubleSpinBox_pox_x->value() * LOEF::millimetre,
                                       arg1 * LOEF::millimetre);
}

void LOEF_individual_fixed_charge_editor::on_remove_button_clicked() {
    emit fixed_charge_destroyed(this->id_);
    this->close();
}
