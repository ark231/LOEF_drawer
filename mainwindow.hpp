#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QListWidget>
#include <QMainWindow>
#include<unordered_map>
#include "general_consts.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_button_add_fixed_charge_clicked();

    void on_list_fixed_charges_itemClicked(QListWidgetItem *item);
    void slot_fixed_charge_destroyed(LOEF::id_type);

   private:
    Ui::MainWindow *ui;
    std::unordered_map<LOEF::id_type,QListWidgetItem*> id_to_item;
};
#endif  // MAINWINDOW_HPP
