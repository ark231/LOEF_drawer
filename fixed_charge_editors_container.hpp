#ifndef FIXED_CHARGE_EDITORS_CONTAINER_HPP
#define FIXED_CHARGE_EDITORS_CONTAINER_HPP

#include <QBoxLayout>
#include <QWidget>
#include <utility>
#include <vector>
#include<unordered_map>

#include "general_consts.hpp"
#include "loef_individual_fixed_charge_editor.hpp"
namespace Ui {
class fixed_charge_editors_container;
}

class fixed_charge_editors_container : public QWidget {
    Q_OBJECT

   public:
    explicit fixed_charge_editors_container(QWidget *parent = nullptr);
    ~fixed_charge_editors_container();
    void add_editor(LOEF_individual_fixed_charge_editor *);

   private:
    Ui::fixed_charge_editors_container *ui;
    std::unordered_map<size_t,LOEF_individual_fixed_charge_editor*> editors;
    std::vector<std::pair<LOEF::id_type, size_t>> id_to_index;
    size_t next_index = 0;

   private slots:
    void slot_fixed_charge_destroyed(LOEF::id_type);
};

#endif  // FIXED_CHARGE_EDITORS_CONTAINER_HPP
