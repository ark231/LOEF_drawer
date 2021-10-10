#include "mainwindow.hpp"

#include <QBoxLayout>
#include <QString>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"
#include "loef_individual_fixed_charge_editor.hpp"
#include "qt_consts.hpp"

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

void MainWindow::on_button_add_fixed_charge_clicked() { add_fixed_charge(); }

void MainWindow::on_list_fixed_charges_itemClicked(QListWidgetItem *item) {
    if (this->current_selected_editor) {
        this->current_selected_editor->close();
    }
    auto id_selected_charge = item->data(static_cast<int>(LOEF::item_data_role::id_role)).value<LOEF::id_type>();
    auto selected_charge_infos = ui->loef_drawer->get_fixed_charge_info(id_selected_charge);
    auto selected_editor =
        new LOEF_individual_fixed_charge_editor(id_selected_charge, std::get<0>(selected_charge_infos),
                                                std::get<1>(selected_charge_infos), std::get<2>(selected_charge_infos));
    this->current_selected_editor = selected_editor;
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
void MainWindow::slot_fixed_charge_destroyed(LOEF::id_type id) {
    delete id_to_item[id];
    id_to_item.erase(id);
}
void MainWindow::add_fixed_charge(const LOEF::coulomb_quantity initial_quantity,
                                  const LOEF::millimetre_quantity initial_X,
                                  const LOEF::millimetre_quantity initial_Y) {
    set_new_fixed_charge(ui->loef_drawer->create_fixed_charge(initial_quantity, initial_X, initial_Y));
}
void MainWindow::add_fixed_charge() { set_new_fixed_charge(ui->loef_drawer->create_fixed_charge()); }
void MainWindow::set_new_fixed_charge(const LOEF::id_type id_new_charge) {
    auto item = new QListWidgetItem(tr("positive charge"));
    item->setData(static_cast<int>(LOEF::item_data_role::id_role), QVariant::fromValue(id_new_charge));
    ui->list_fixed_charges->addItem(item);
    id_to_item[id_new_charge] = item;
}
