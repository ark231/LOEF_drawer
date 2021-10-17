#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QListWidget>
#include <QMainWindow>
#include <optional>
#include <unordered_map>

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

    void on_button_add_fixed_charge_clicked();
    void on_list_fixed_charges_itemClicked(QListWidgetItem *item);
    void on_actionEnglish_triggered(bool arg1);
    void on_actionJapanese_triggered(bool arg1);
    void on_radio_draw_toggled(bool checked);
    void on_button_save_clicked();
    void on_button_open_clicked();

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
