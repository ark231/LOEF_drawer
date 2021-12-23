#include "mainwindow.hpp"

#include <QBoxLayout>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>
#include <QString>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"
#include "loef_drawer.hpp"
#include "loef_individual_fixed_charge_editor.hpp"
#include "qt_consts.hpp"

MainWindow::MainWindow(QLocale locale, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->label_app_info->setText(QStringLiteral("%1 \nv%2.%3.%4")
                                    .arg(LOEF::application_name)
                                    .arg(LOEF::version_major)
                                    .arg(LOEF::version_minor)
                                    .arg(LOEF::version_patch));
    this->setWindowTitle(LOEF::application_name);
    // lazy
    ui->loef_drawer->set_electric_potential(&this->electric_potential_handler);
    // end lazy
    connect(ui->loef_drawer, SIGNAL(fixed_charge_selected(LOEF::id_type)), this,
            SLOT(slot_fixed_charge_selected(LOEF::id_type)));
    connect(ui->doubleSpinBox_inverse_permittivity, SIGNAL(valueChanged(double)), ui->loef_drawer,
            SLOT(slot_inverse_permittivity_changed(double)));
    ui->doubleSpinBox_inverse_permittivity->setValue(LOEF::initial_inverse_permittivity.value());
    if (locale == QLocale("en")) {
        ui->actionEnglish->setChecked(true);
    } else if (locale == QLocale("ja")) {
        ui->actionJapanese->setChecked(true);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_button_add_fixed_charge_clicked() { add_fixed_charge(); }

void MainWindow::on_list_fixed_charges_itemClicked(QListWidgetItem *item) {
    auto id_selected_charge = item->data(static_cast<int>(LOEF::item_data_role::id_role)).value<LOEF::id_type>();
    auto selected_charge_infos = ui->loef_drawer->get_fixed_charge_info(id_selected_charge);
    ui->loef_drawer->select_fixed_charge(id_selected_charge);
    if (this->current_selected_editor_) {
        ui->loef_drawer->unselect_fixed_charge(current_selected_editor_->id());
        emit fixed_charge_select_changed(id_selected_charge, std::get<0>(selected_charge_infos),
                                         std::get<1>(selected_charge_infos), std::get<2>(selected_charge_infos));
        return;
    }
    auto selected_editor =
        new LOEF_individual_fixed_charge_editor(id_selected_charge, std::get<0>(selected_charge_infos),
                                                std::get<1>(selected_charge_infos), std::get<2>(selected_charge_infos));
    this->current_selected_editor_ = selected_editor;
    selected_editor->setAttribute(Qt::WA_DeleteOnClose);
    if (fixed_charge_editor_last_pos_ != LOEF::invalid_position) {
        selected_editor->move(fixed_charge_editor_last_pos_);
    }
    connect(
        ui->loef_drawer,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        selected_editor,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    connect(
        selected_editor,
        SIGNAL(fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)),
        ui->loef_drawer,
        SLOT(slot_fixed_charge_position_changed(LOEF::id_type, LOEF::millimetre_quantity, LOEF::millimetre_quantity)));
    connect(selected_editor, SIGNAL(fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)),
            ui->loef_drawer, SLOT(slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)));
    connect(selected_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), ui->loef_drawer,
            SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    connect(selected_editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), this,
            SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    connect(selected_editor, SIGNAL(fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)), this,
            SLOT(slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity)));
    connect(selected_editor, SIGNAL(editor_fixed_charge_closed(QPoint)), this,
            SLOT(slot_editor_fixed_charge_closed(QPoint)));
    connect(this,
            SIGNAL(fixed_charge_select_changed(LOEF::id_type, LOEF::coulomb_quantity, LOEF::millimetre_quantity,
                                               LOEF::millimetre_quantity)),
            selected_editor,
            SLOT(slot_fixed_charge_select_changed(LOEF::id_type, LOEF::coulomb_quantity, LOEF::millimetre_quantity,
                                                  LOEF::millimetre_quantity)));

    selected_editor->show();
}
void MainWindow::slot_fixed_charge_destroyed(LOEF::id_type id) {
    delete id_to_item_[id];
    id_to_item_.erase(id);
    current_selected_editor_ = nullptr;
}
void MainWindow::add_fixed_charge(const LOEF::coulomb_quantity initial_quantity,
                                  const LOEF::millimetre_quantity initial_X,
                                  const LOEF::millimetre_quantity initial_Y) {
    set_new_fixed_charge(ui->loef_drawer->create_fixed_charge(initial_quantity, initial_X, initial_Y));
}
void MainWindow::add_fixed_charge() { set_new_fixed_charge(ui->loef_drawer->create_fixed_charge()); }
void MainWindow::set_new_fixed_charge(const LOEF::id_type id_new_charge) {
    QString charge_list_item_title;
    auto new_charge_quantity = std::get<0>(ui->loef_drawer->get_fixed_charge_info(id_new_charge));
    if (new_charge_quantity > 0.0 * LOEF::boostunits::coulomb) {
        charge_list_item_title = tr("positive charge");
    } else if (new_charge_quantity == 0.0 * LOEF::boostunits::coulomb) {
        charge_list_item_title = tr("null charge");
    } else {
        charge_list_item_title = tr("negative charge");
    }
    auto item = new QListWidgetItem(charge_list_item_title);
    item->setData(static_cast<int>(LOEF::item_data_role::id_role), QVariant::fromValue(id_new_charge));
    ui->list_fixed_charges->addItem(item);
    id_to_item_[id_new_charge] = item;
}
void MainWindow::slot_fixed_charge_charge_changed(LOEF::id_type id, LOEF::coulomb_quantity new_quantity) {
    if (new_quantity > 0.0 * LOEF::boostunits::coulomb) {
        id_to_item_[id]->setText(tr("positive charge"));
    } else if (new_quantity == 0.0 * LOEF::boostunits::coulomb) {
        id_to_item_[id]->setText(tr("null charge"));
    } else if (new_quantity < 0.0 * LOEF::boostunits::coulomb) {
        id_to_item_[id]->setText(tr("negative charge"));
    }
}
void MainWindow::slot_fixed_charge_selected(LOEF::id_type id) {
    if (current_selected_editor_) {
        auto selected_item = id_to_item_[id];
        this->ui->list_fixed_charges->scrollToItem(selected_item);
        this->on_list_fixed_charges_itemClicked(selected_item);
    } else {
        this->ui->loef_drawer->unselect_all_selected_fixed_charge();
    }
}

void MainWindow::on_actionEnglish_triggered(bool arg1) {
    if (arg1) {
        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings.ini", QSettings::IniFormat);
        settings.beginGroup("init");
        settings.setValue("locale", QLocale("en"));
        settings.endGroup();
        settings.sync();
        if (confirm_restart()) {
            restart();
        }
    }
    this->ui->actionJapanese->setChecked(!arg1);  // sadly, actiongroup didn't work well..
}

void MainWindow::on_actionJapanese_triggered(bool arg1) {
    if (arg1) {
        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings.ini", QSettings::IniFormat);
        settings.beginGroup("init");
        settings.setValue("locale", QLocale("ja"));
        settings.endGroup();
        settings.sync();
        if (confirm_restart()) {
            restart();
        }
    }
    this->ui->actionEnglish->setChecked(!arg1);
}

bool MainWindow::confirm_restart() {
    QMessageBox msgbox(this);
    msgbox.setText(tr("This change require restarting this program.\nDo you want to restart this program?"));
    msgbox.setWindowTitle(tr("confirm restart"));
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::Yes);
    msgbox.setIcon(QMessageBox::Question);
    bool result = false;
    switch (msgbox.exec()) {
        case QMessageBox::Yes:
            result = true;
            break;
        case QMessageBox::No:
            result = false;
            break;
    }
    return result;
}

void MainWindow::restart() { QCoreApplication::exit(LOEF::restart_code); }
void MainWindow::slot_editor_fixed_charge_closed(QPoint pos) {
    this->current_selected_editor_ = nullptr;
    this->fixed_charge_editor_last_pos_ = pos;
    this->ui->loef_drawer->unselect_all_selected_fixed_charge();
}
void MainWindow::closeEvent(QCloseEvent *) { QApplication::closeAllWindows(); }

void MainWindow::on_radio_draw_toggled(bool checked) { this->ui->loef_drawer->request_draw_LOEF(checked); }

void save_to_file(QFile &savefile, QJsonObject &save_data);
void MainWindow::on_button_save_clicked() {
    QJsonObject save_data;
    save_data = ui->loef_drawer->create_save_data();
    bool save_confirmed;
    QString save_name = QInputDialog::getText(this, tr("enter save name"), tr("save name:"), QLineEdit::Normal,
                                              tr("untitled"), &save_confirmed);
    bool successfully_saved = false;
    if (save_confirmed && !save_name.isEmpty()) {
        while (!successfully_saved) {
            QFile save_file(QCoreApplication::applicationDirPath() + "/saves/" + save_name + ".json");
            if (!save_file.exists()) {
                save_to_file(save_file, save_data);
                successfully_saved = true;  // breakするので意味はないが、、、
                break;
            } else {
                QMessageBox msgbox(this);
                msgbox.setText(tr("save file with entered name already exists.\ndo you want to overwrite it?"));
                msgbox.setWindowTitle(tr("save file exists"));
                msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgbox.setDefaultButton(QMessageBox::No);
                msgbox.setIcon(QMessageBox::Warning);
                switch (msgbox.exec()) {
                    case QMessageBox::Yes:
                        save_to_file(save_file, save_data);
                        successfully_saved = true;
                        break;
                    case QMessageBox::No:
                        save_name = QInputDialog::getText(this, tr("enter save name"), tr("save name:"),
                                                          QLineEdit::Normal, tr("untitled"), &save_confirmed);
                        successfully_saved = false;
                        break;
                }
            }
        }
    }
}
void save_to_file(QFile &savefile, QJsonObject &save_data) {
    savefile.open(QIODevice::WriteOnly);  //失敗処理の実装は後回し
    savefile.write(QJsonDocument(save_data).toJson());
}

void MainWindow::on_button_open_clicked() {
    auto saved_filename = QFileDialog::getOpenFileName(
        this, tr("open save file"), QCoreApplication::applicationDirPath() + "/saves", tr("JSON files (*.json)"));
    Q_ASSERT(QFile::exists(saved_filename));
    QFile saved_file(saved_filename);
    saved_file.open(QIODevice::ReadOnly);
    QJsonDocument saved_json(QJsonDocument::fromJson(saved_file.readAll()));
    QJsonObject saved_data = saved_json.object();
    for (auto &id_item : id_to_item_) {
        delete id_item.second;
    }
    id_to_item_.clear();
    ui->loef_drawer->destroy_all_fixed_charges();
    ui->doubleSpinBox_inverse_permittivity->setValue(saved_data["inverse permittivity"].toDouble());
    for (const auto &json_fixed_charge : saved_data["fixed_charges"].toArray()) {
        auto json_object_fixed_charge = json_fixed_charge.toObject();
        LOEF::coulomb_quantity saved_charge_quantity =
            json_object_fixed_charge["charge quantity"].toDouble() * LOEF::boostunits::coulomb;
        QJsonObject json_position = json_object_fixed_charge["position"].toObject();
        LOEF::millimetre_quantity saved_position_x = json_position["x"].toDouble() * LOEF::millimetre;
        LOEF::millimetre_quantity saved_position_y = json_position["y"].toDouble() * LOEF::millimetre;
        this->add_fixed_charge(saved_charge_quantity, saved_position_x, saved_position_y);
    }
}

void MainWindow::on_actionenable_epcolor_toggled(bool arg1) { this->electric_potential_handler.color_enabled = arg1; }

void MainWindow::on_actionpositive_triggered() {
    if (this->electric_potential_handler.color_use_input) {
        auto input_max_abs_positive =
            QInputDialog::getDouble(this, tr("max absolute positive potential"), tr("enter max abs positive"),
                                    this->electric_potential_handler.get_current_max_abs_positive().value(), 0);
        this->electric_potential_handler.set_current_max_abs_positive(input_max_abs_positive * LOEF::boostunits::volt);
    }
}

void MainWindow::on_actionnegative_triggered() {
    if (this->electric_potential_handler.color_use_input) {
        auto input_max_abs_negative = QInputDialog::getDouble(
            this, tr("max absolute negative potential"), tr("enter max abs negative"),
            boost::units::abs(this->electric_potential_handler.get_current_max_abs_negative()).value(), 0);
        this->electric_potential_handler.set_current_max_abs_negative(-input_max_abs_negative * LOEF::boostunits::volt);
    }
}

void MainWindow::on_actionenable_epsurface_toggled(bool arg1) {
    this->electric_potential_handler.surface_enabled = arg1;
}

void MainWindow::on_actiondistance_triggered() {
    auto input_distance =
        QInputDialog::getDouble(this, tr("distance between equipotential surfaces"), tr("enter distance"), 0, 0);
    this->electric_potential_handler.distance = input_distance * LOEF::boostunits::volt;
}

void MainWindow::on_actiondisable_LOEF_toggled(bool arg1) { this->electric_potential_handler.disable_LOEF = arg1; }

void MainWindow::on_actionuse_input_toggled(bool arg1) { this->electric_potential_handler.color_use_input = arg1; }

void MainWindow::on_actionmax_error_triggered() {
    auto input_max_error = QInputDialog::getDouble(this, tr("max_error"), tr("enter max_error"), 0, 0);
    LOEF::experimental::max_error_surface = input_max_error;
}
