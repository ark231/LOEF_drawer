#include "loef_drawer.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

#include "LOEF_QPainter.hpp"

LOEF_drawer::LOEF_drawer(QWidget *parent) : QWidget(parent) {
    double dpi = QGuiApplication::screens().at(0)->physicalDotsPerInch();
    this->dpmm_ = dpi / 25.4;
    fixed_charges.push_back(LOEF::fixed_charge(+1, 200, 300));
    fixed_charges.push_back(LOEF::fixed_charge(+0, 400, 300));
    fixed_charges.push_back(LOEF::fixed_charge(-1, 600, 300));
}
void LOEF_drawer::paintEvent(QPaintEvent *ev) {
    LOEF::painter painter(this);
    painter.set_resolution(dpmm_);
    auto width = this->width();
    auto height = this->height();
    painter.drawLine(0, height / 2.0, width, height / 2.0);
    painter.drawLine(width / 2.0, 0, width / 2.0, height);
    for (const auto &charge : fixed_charges) {
        painter.draw_fixed_charge(charge);
    }
}
