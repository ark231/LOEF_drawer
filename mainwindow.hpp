#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QListWidget>
#include <QMainWindow>
#include <optional>
#include <unordered_map>

#include "general_consts.hpp"

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

   private slots:
    void on_button_add_fixed_charge_clicked();

    void on_list_fixed_charges_itemClicked(QListWidgetItem *item);
    void slot_fixed_charge_charge_changed(LOEF::id_type, LOEF::coulomb_quantity);
    void slot_fixed_charge_destroyed(LOEF::id_type);
    void slot_fixed_charge_selected(LOEF::id_type);

    void on_actionEnglish_triggered(bool arg1);

    void on_actionJapanese_triggered(bool arg1);

   private:
    Ui::MainWindow *ui;
    std::unordered_map<LOEF::id_type, QListWidgetItem *> id_to_item_;
    LOEF_individual_fixed_charge_editor *current_selected_editor_ = nullptr;
    void add_fixed_charge(const LOEF::coulomb_quantity initial_quantity, const LOEF::millimetre_quantity initial_X,
                          const LOEF::millimetre_quantity initial_Y);
    void add_fixed_charge();
    void set_new_fixed_charge(const LOEF::id_type);
    bool confirm_restart();
    void restart();
};
#endif  // MAINWINDOW_HPP
