#include "loef_drawer.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QScreen>
#include <iterator>

#include "LOEF_QPainter.hpp"
namespace LOEF {
class state_charge_selected_ {
    bool exists_selected_ = false;
    size_t index_selected_charge_ = -1;  // this must be super big number ,as so much instances can't be stored
    vec2d offset_ = {0, 0};

   public:
    state_charge_selected_();
    operator bool();
    void set_selected(size_t idx, vec2d offset);
    void unselected();
    size_t get_selected();
    vec2d get_offset();
};
state_charge_selected_::state_charge_selected_() {
    exists_selected_ = false;
    index_selected_charge_ = -1;
    offset_ = {0, 0};
}
state_charge_selected_::operator bool() { return exists_selected_; }
void state_charge_selected_::set_selected(size_t idx, vec2d offset) {
    exists_selected_ = true;
    index_selected_charge_ = idx;
    offset_ = offset;
}
void state_charge_selected_::unselected() {
    exists_selected_ = false;
    index_selected_charge_ = -1;
}
size_t state_charge_selected_::get_selected() { return index_selected_charge_; }
vec2d state_charge_selected_::get_offset() { return offset_; }
}  // namespace LOEF

LOEF_drawer::LOEF_drawer(QWidget *parent) : QWidget(parent) {
    double dpi = QGuiApplication::screens().at(0)->physicalDotsPerInch();
    this->dpmm_ = dpi / 25.4;
    charge_selected_ = new LOEF::state_charge_selected_;
    fixed_charges_.push_back(LOEF::fixed_charge(+1, 200, 300));
    fixed_charges_.push_back(LOEF::fixed_charge(+0, 400, 300));
    fixed_charges_.push_back(LOEF::fixed_charge(-1, 600, 300));
}
void LOEF_drawer::paintEvent(QPaintEvent *ev) {
    LOEF::painter painter(this);
    painter.set_resolution(dpmm_);
    auto width = this->width();
    auto height = this->height();
    painter.drawLine(0, height / 2.0, width, height / 2.0);
    painter.drawLine(width / 2.0, 0, width / 2.0, height);
    for (const auto &charge : fixed_charges_) {
        painter.draw_fixed_charge(charge);
    }
}
void LOEF_drawer::mousePressEvent(QMouseEvent *ev) {
    LOEF::vec2d pos_mouse(ev->pos());
    for (auto charge = fixed_charges_.begin(); charge != fixed_charges_.end(); charge++) {
        auto offset = pos_mouse - charge->position();
        if (offset.length() <= LOEF::radius::FIXED * dpmm_) {
            charge_selected_->set_selected(std::distance(fixed_charges_.begin(), charge), offset);
        }
    }
}
void LOEF_drawer::mouseMoveEvent(QMouseEvent *ev) {
    if (*charge_selected_) {
        auto idx_selected = charge_selected_->get_selected();
        auto old_charge = fixed_charges_[idx_selected];
        fixed_charges_[idx_selected] =
            LOEF::fixed_charge(old_charge.quantity(), ev->pos() + charge_selected_->get_offset());
        update();
    }
}
void LOEF_drawer::mouseReleaseEvent(QMouseEvent *ev) { charge_selected_->unselected(); }
