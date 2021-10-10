#include "fixed_charge_editors_container.hpp"

#include <QDebug>

#include "ui_fixed_charge_editors_container.h"

fixed_charge_editors_container::fixed_charge_editors_container(QWidget* parent)
    : QWidget(parent), ui(new Ui::fixed_charge_editors_container) {
    ui->setupUi(this);
}

fixed_charge_editors_container::~fixed_charge_editors_container() { delete ui; }

void fixed_charge_editors_container::add_editor(LOEF_individual_fixed_charge_editor* editor) {
    QObject::connect(editor, SIGNAL(fixed_charge_destroyed(LOEF::id_type)), this,
                     SLOT(slot_fixed_charge_destroyed(LOEF::id_type)));
    // this->setMinimumSize(editor->width(), this->height() + editor->height());
    this->ui->verticalLayout->addWidget(editor);
    this->setMinimumSize(editor->width(), editor->height() * (editors.size() + 1));
    /*
    editor->setParent(this);
    editor->move(editor->x(), editor->y() + this->id_to_index.size() * editor->height());
    editor->show();
    */
    id_to_index.push_back(std::make_pair(editor->id(), next_index));
    editors[next_index] = editor;
    next_index++;
}
void fixed_charge_editors_container::slot_fixed_charge_destroyed(LOEF::id_type id) {
    bool is_below_destroyed = false;
    size_t idx_id_index_to_erase = 0;
    for (auto index_pair : id_to_index) {
        // qDebug() << "        @fixed_editor_container slot_destroyed id:" << id
        //         << "index_pair.first:" << index_pair.first;
        if (index_pair.first == id) {
            editors.erase(index_pair.second);
            is_below_destroyed = true;
        } else if (is_below_destroyed) {
            auto current_editor = editors[index_pair.second];
            current_editor->move(current_editor->x(), current_editor->y() - current_editor->height());
        } else {
            idx_id_index_to_erase++;
        }
    }
    id_to_index.erase(id_to_index.begin() + idx_id_index_to_erase);
    if (editors.empty()) {
        this->setMinimumHeight(0);
    } else {
        // this->setMinimumHeight(this->height() - editors.begin()->second->height());
        this->setMinimumHeight(editors.begin()->second->height() * editors.size());
    }
}
