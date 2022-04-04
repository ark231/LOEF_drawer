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
#include <QTextStream>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"
#include "loef_drawer.hpp"
#include "loef_individual_fixed_charge_editor.hpp"
#include "qt_consts.hpp"
#include "qt_toml_settings.hpp"

MainWindow::MainWindow(QLocale locale, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->label_app_info->setText(QStringLiteral("%1 \nv%2.%3.%4%5")
                                    .arg(LOEF::application_name)
                                    .arg(LOEF::version_major)
                                    .arg(LOEF::version_minor)
                                    .arg(LOEF::version_patch)
                                    .arg(LOEF::version_suffix));
    this->setWindowTitle(LOEF::application_name);
    connect(ui->loef_drawer, SIGNAL(fixed_charge_selected(LOEF::id_type)), this,
            SLOT(slot_fixed_charge_selected(LOEF::id_type)));
    connect(ui->loef_drawer, SIGNAL(editor_fixed_charge_open_requested(LOEF::id_type)), this,
            SLOT(slot_editor_fixed_charge_open_requested(LOEF::id_type)));
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
void MainWindow::slot_editor_fixed_charge_open_requested(LOEF::id_type id) {
    for (auto i = 0; i < ui->list_fixed_charges->count(); i++) {
        auto item = ui->list_fixed_charges->item(i);
        auto id_item = item->data(static_cast<int>(LOEF::item_data_role::id_role)).value<LOEF::id_type>();
        if (id_item == id) {
            this->on_list_fixed_charges_itemClicked(item);
            break;
        }
    }
}

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
        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings." + LOEF::configfile_ext,
                           LOEF_QT_CONFIG_FORMAT);
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
        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings." + LOEF::configfile_ext,
                           LOEF_QT_CONFIG_FORMAT);
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
    msgbox.setText(
        tr("This change require restarting this program.\nDo you want "
           "to restart this program?"));
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
                msgbox.setText(
                    tr("save file with entered name already exists.\ndo you "
                       "want to overwrite it?"));
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
    if (saved_data.contains("fixed_charge_radius")) {  //互換性維持のためチェック
        LOEF::radius::FIXED = saved_data["fixed_charge_radius"].toDouble() * LOEF::millimetre;
    } else {
        LOEF::radius::FIXED = LOEF::default_val::radius::FIXED;
    }
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

// lazy
void MainWindow::on_actionenable_epcolor_toggled(bool arg1) { ui->loef_drawer->ep_handler.color_enabled = arg1; }

void MainWindow::on_actionpositive_triggered() {
    if (ui->loef_drawer->ep_handler.color_use_input) {
        auto input_max_abs_positive =
            QInputDialog::getDouble(this, tr("max absolute positive potential"), tr("enter max abs positive"),
                                    ui->loef_drawer->ep_handler.get_current_max_abs_positive().value(), 0);
        ui->loef_drawer->ep_handler.set_current_max_abs_positive(input_max_abs_positive * LOEF::boostunits::volt);
    }
}

void MainWindow::on_actionnegative_triggered() {
    if (ui->loef_drawer->ep_handler.color_use_input) {
        auto input_max_abs_negative = QInputDialog::getDouble(
            this, tr("max absolute negative potential"), tr("enter max abs negative"),
            boost::units::abs(ui->loef_drawer->ep_handler.get_current_max_abs_negative()).value(), 0);
        ui->loef_drawer->ep_handler.set_current_max_abs_negative(-input_max_abs_negative * LOEF::boostunits::volt);
    }
}

void MainWindow::on_actionenable_epsurface_toggled(bool arg1) { ui->loef_drawer->ep_handler.surface_enabled = arg1; }

void MainWindow::on_actiondistance_triggered() {
    auto input_distance =
        QInputDialog::getDouble(this, tr("distance between equipotential surfaces"), tr("enter distance"),
                                ui->loef_drawer->ep_handler.distance.value(), 0);
    ui->loef_drawer->ep_handler.distance = input_distance * LOEF::boostunits::volt;
}

void MainWindow::on_actiondisable_LOEF_toggled(bool arg1) { ui->loef_drawer->ep_handler.disable_LOEF = arg1; }

void MainWindow::on_actionuse_input_toggled(bool arg1) {
    ui->actionpositive->setEnabled(arg1);
    ui->actionnegative->setEnabled(arg1);
    ui->loef_drawer->ep_handler.color_use_input = arg1;
}

void MainWindow::on_actionmax_error_triggered() {
    auto input_max_error =
        QInputDialog::getDouble(this, tr("max_error"), tr("enter max_error"), LOEF::experimental::max_error_surface, 0);
    LOEF::experimental::max_error_surface = input_max_error;
}
using LOEF::experimental::mm;
void MainWindow::on_actionoutput_samples_triggered() {
    QMessageBox::information(this, tr("output samples"),
                             tr("2塊の非荷電電荷の間の電位のサンプルを出力\n電荷"
                                "が2つ重なってるほうが終端"));
    LOEF::millimetre_quantity distance =
        QInputDialog::getDouble(this, tr("distance"), tr("enter distance"), 1.0, 0, 2147483647, 10) * LOEF::millimetre;
    QString output_filename =
        QInputDialog::getText(this, tr("enter save name"), tr("save name:"), QLineEdit::Normal, tr("untitled"));
    QFile output_file(QCoreApplication::applicationDirPath() + "/saves/" + output_filename + ".csv");
    output_file.open(QIODevice::WriteOnly);  //失敗処理の実装は後回し
    std::vector<LOEF::vec2d> line_ends;
    auto fixed_charges = this->ui->loef_drawer->get_fixed_charges();
    for (const auto &fixed_charge : fixed_charges) {
        if (fixed_charge.quantity().value() == 0.0) {
            line_ends.push_back(fixed_charge.position());
        }
    }
    if (line_ends.size() == 3) {
        LOEF::vec2d start;
        LOEF::vec2d end;
        auto diff_01 = (line_ends[0] - line_ends[1]).length().value();
        auto diff_02 = (line_ends[0] - line_ends[2]).length().value();
        auto diff_12 = (line_ends[1] - line_ends[2]).length().value();
        auto min_diff = std::min({diff_01, diff_02, diff_12});
        if (min_diff == diff_01) {
            start = line_ends[2];
            end = (line_ends[0] + line_ends[1]) / 2.0;
        } else if (min_diff == diff_02) {
            start = line_ends[1];
            end = (line_ends[0] + line_ends[2]) / 2.0;
        } else if (min_diff == diff_12) {
            start = line_ends[0];
            end = (line_ends[1] + line_ends[2]) / 2.0;
        } else {
            output_file.write("");
            QMessageBox::warning(this, tr("output samples"), tr("couldn't make vector"));
            output_file.close();
            return;
        }
        QTextStream output_stream(&output_file);
        for (auto from_start = LOEF::vec2d(0 * mm, 0 * mm); from_start.length() <= (end - start).length();
             from_start += (normalize(end - start) * distance.value())) {
            LOEF::volt_quantity potential;
            for (const auto &fixed_charge : fixed_charges) {
                auto charge_to_pos = fixed_charge.position() - (start + from_start);
                potential += LOEF::experimental::k0 * fixed_charge.quantity() /
                             static_cast<LOEF::metre_quantity>(charge_to_pos.length());
            }
            output_stream << from_start.length().value() << "," << potential.value() << "\n";
        }
    } else if (line_ends.size() == 2) {
        QTextStream output_stream(&output_file);
        output_stream << "#x,y,potential\n";
        auto start_x = boost::units::fmin(line_ends[0].x(), line_ends[1].x());
        auto start_y = boost::units::fmin(line_ends[0].y(), line_ends[1].y());
        auto end_x = boost::units::fmax(line_ends[0].x(), line_ends[1].x());
        auto end_y = boost::units::fmax(line_ends[0].y(), line_ends[1].y());
        for (auto y = start_y; y <= end_y; y += distance) {
            for (auto x = start_x; x <= end_x; x += distance) {
                LOEF::volt_quantity potential;
                for (const auto &fixed_charge : fixed_charges) {
                    auto charge_to_pos = fixed_charge.position() - LOEF::vec2d(x, y);
                    potential += LOEF::experimental::k0 * fixed_charge.quantity() /
                                 static_cast<LOEF::metre_quantity>(charge_to_pos.length());
                }
                output_stream << x.value() << "," << y.value() << "," << potential.value() << "\n";
            }
        }
    } else {
        QMessageBox::warning(this, tr("output samples"),
                             tr("you must specify line or rectangle by placing three or two null charges."));
        output_file.close();
        return;
    }
    output_file.close();
    QMessageBox::information(this, tr("output samples"), tr("output successfully ends"));
}

void MainWindow::on_actionshow_line_toggled(bool arg1) { ui->loef_drawer->set_is_draw_sample_line_requested(arg1); }
// 両方選択してもOK

void MainWindow::on_actionshow_rectangle_toggled(bool arg1) {
    ui->loef_drawer->set_is_draw_sample_rectangle_requested(arg1);
}
// end lazy

void MainWindow::on_actionuse_ready_made_algorithm_triggered(bool checked) {
    this->ui->loef_drawer->set_is_ready_made_requested(checked);
}
void MainWindow::on_actionabout_qt_triggered() { QMessageBox::aboutQt(this, tr("about Qt")); }

void MainWindow::on_actionabout_LOEF_drawer_triggered() {
    QString locale_suffix = "";
    if (this->ui->actionJapanese->isChecked()) {
        locale_suffix = "ja";
    } else if (this->ui->actionEnglish->isChecked()) {
        locale_suffix = "en";
    }
    QFile description_file(QStringLiteral(":/res/text/about_LOEF_drawer_%1.html").arg(locale_suffix));
    // qDebug() << description_file.fileName() << "exists()==" << description_file.exists();
    if (not description_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("failed to open resource"),
                             tr("failed to open resource %1").arg(description_file.fileName()));
        return;
    }
    auto description = QTextStream(&description_file).readAll();
    auto version = QString("%1.%2.%3%4")
                       .arg(LOEF::version_major)
                       .arg(LOEF::version_minor)
                       .arg(LOEF::version_patch)
                       .arg(LOEF::version_suffix);
    // qDebug() << description;
    QMessageBox::about(this, tr("about LOEF_drawer"),
                       description.replace("LOEF_VERSION", version).replace("LOEF_BUILD_TYPE", LOEF::build_type));
}
