#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QListWidget>
#include <QMainWindow>
#include <optional>
#include <unordered_map>

#include "experimental/electric_potential.hpp"
#include "general_consts.hpp"
#include "qt_consts.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class LOEF_individual_fixed_charge_editor;
class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QLocale, QWidget *parent = nullptr);
    ~MainWindow();

   signals:
    void fixed_charge_select_changed(LOEF::id_type, LOEF::coulomb_quantity, LOEF::millimetre_quantity,
                                     LOEF::millimetre_quantity);

   private slots:
    void slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity);
    void slot_fixed_charge_destroyed(LOEF::id_type);
    void slot_fixed_charge_selected(LOEF::id_type);
    void slot_editor_fixed_charge_closed(QPoint);
    void slot_editor_fixed_charge_open_requested(LOEF::id_type);

    void on_button_add_fixed_charge_clicked();
    void on_list_fixed_charges_itemClicked(QListWidgetItem *item);
    void on_actionEnglish_triggered(bool arg1);
    void on_actionJapanese_triggered(bool arg1);
    void on_radio_draw_toggled(bool checked);
    void on_button_save_clicked();
    void on_button_open_clicked();

    // lazy
    void on_actionenable_epcolor_toggled(bool arg1);
    void on_actionpositive_triggered();
    void on_actionnegative_triggered();
    void on_actionenable_epsurface_toggled(bool arg1);
    void on_actiondistance_triggered();
    void on_actiondisable_LOEF_toggled(bool arg1);
    void on_actionuse_input_toggled(bool arg1);
    void on_actionmax_error_triggered();
    void on_actionoutput_samples_triggered();
    void on_actionshow_line_toggled(bool arg1);
    void on_actionshow_rectangle_toggled(bool arg1);
    void on_actionuse_ready_made_algorithm_triggered(bool checked);
    void on_actionalgorythm_type_triggered();
    // end lazy
    void on_actionabout_qt_triggered();

    void on_actionabout_LOEF_drawer_triggered();

    void on_actionside_length_triggered();

private:
    Ui::MainWindow *ui;
    std::unordered_map<LOEF::id_type, QListWidgetItem *> id_to_item_;
    LOEF_individual_fixed_charge_editor *current_selected_editor_ = nullptr;
    QPoint fixed_charge_editor_last_pos_ = LOEF::invalid_position;

   private:
    void add_fixed_charge(const LOEF::coulomb_quantity initial_quantity, const LOEF::millimetre_quantity initial_X,
                          const LOEF::millimetre_quantity initial_Y);
    void add_fixed_charge();
    void set_new_fixed_charge(const LOEF::id_type);
    bool confirm_restart();
    void restart();
    void closeEvent(QCloseEvent *) override;
};
#endif  // MAINWINDOW_HPP
