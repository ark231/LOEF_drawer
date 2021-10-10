#include "mainwindow.hpp"

#include <QBoxLayout>
#include <QString>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"
#include"qt_consts.hpp"
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

void MainWindow::on_button_add_fixed_charge_clicked() {
    auto id_new_charge = ui->loef_drawer->create_fixed_charge();
    auto item = new QListWidgetItem(tr("positive charge"));
    item->setData(static_cast<int>(LOEF::item_data_role::id_role),QVariant::fromValue(id_new_charge));
    ui->list_fixed_charges->addItem(item);
    id_to_item[id_new_charge]=item;
}

void MainWindow::on_list_fixed_charges_itemClicked(QListWidgetItem *item) {
    auto id_selected_charge =item->data(static_cast<int>(LOEF::item_data_role::id_role)).value<LOEF::id_type>();
    auto selected_editor = new LOEF_individual_fixed_charge_editor(id_selected_charge);
    QObject::connect(
        ui->loef_drawer,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        selected_editor,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    QObject::connect(
        selected_editor,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        ui->loef_drawer,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    QObject::connect(selected_editor, SIGNAL(fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)),
                     ui->loef_drawer, SLOT(slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)));
    QObject::connect(selected_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), ui->loef_drawer,
                     SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    QObject::connect(selected_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), this,
                     SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    selected_editor->show();
}
void MainWindow::slot_fixed_charge_destroyed(LOEF::id_type id){
    delete id_to_item[id];
    id_to_item.erase(id);
}
