#include "loef_individual_fixed_charge_editor.hpp"

#include <QMoveEvent>

#include "ui_loef_individual_fixed_charge_editor.h"

LOEF_individual_fixed_charge_editor::LOEF_individual_fixed_charge_editor(LOEF::id_type id,
                                                                         LOEF::coulomb_quantity initial_charge,
                                                                         LOEF::millimetre_quantity initial_X,
                                                                         LOEF::millimetre_quantity initial_Y,
                                                                         QWidget* parent)
    : QWidget(parent), ui(new Ui::LOEF_individual_fixed_charge_editor) {
    ui->setupUi(this);
    this->id_ = id;
    ui->doubleSpinBox_pox_x->setValue(initial_X.value());
    ui->doubleSpinBox_pox_y->setValue(initial_Y.value());
    ui->doubleSpinBox_quantity->setValue(initial_charge.value());
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
void LOEF_individual_fixed_charge_editor::closeEvent(QCloseEvent*) {
    QPoint pos_to_send = LOEF::invalid_position;
    if (moved_pos_ == LOEF::invalid_position) {
        pos_to_send = this->pos();
    } else {
        pos_to_send = this->moved_pos_;
    }
    emit editor_fixed_charge_closed(pos_to_send);
}
void LOEF_individual_fixed_charge_editor::moveEvent(QMoveEvent* ev) { this->moved_pos_ = ev->pos(); }
