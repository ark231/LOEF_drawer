#ifndef LOEF_DRAWER_HPP
#define LOEF_DRAWER_HPP

#include <QWidget>
#include <vector>

#include "charges.hpp"

class LOEF_drawer : public QWidget {
    Q_OBJECT
   public:
    explicit LOEF_drawer(QWidget *parent = nullptr);

   private:
    double dpmm_;
    std::vector<LOEF::fixed_charge> fixed_charges;
    void paintEvent(QPaintEvent *ev) override;

   signals:
};

#endif  // LOEF_DRAWER_HPP
