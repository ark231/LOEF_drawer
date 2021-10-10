#include "mainwindow.hpp"

#include <QBoxLayout>
#include <QString>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"
#include "loef_individual_fixed_charge_editor.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->label_app_info->setText(QStringLiteral("%1 \nv%2.%3.%4")
                                    .arg(LOEF::application_name)
                                    .arg(LOEF::version_major)
                                    .arg(LOEF::version_minor)
                                    .arg(LOEF::version_patch));
    this->setWindowTitle(LOEF::application_name);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked() {
    auto id_new_charge = ui->loef_drawer->create_fixed_charge();
    auto new_editor = new LOEF_individual_fixed_charge_editor(id_new_charge);
    QObject::connect(
        ui->loef_drawer,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        new_editor,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    QObject::connect(
        new_editor,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        ui->loef_drawer,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    QObject::connect(new_editor, SIGNAL(fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)),
                     ui->loef_drawer, SLOT(slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)));
    QObject::connect(new_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), ui->loef_drawer,
                     SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    QObject::connect(new_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)),
                     ui->fixed_charge_editors_container_instance, SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    ui->fixed_charge_editors_container_instance->add_editor(new_editor);
    // ui->fixed_editors_layout->addWidget(new_editor);
}
