#ifndef LOEF_DRAWER_HPP
#define LOEF_DRAWER_HPP

#include <QWidget>
#include <vector>

#include "charges.hpp"
#include "vec2d.hpp"
namespace LOEF {
class state_charge_selected_;
}  // namespace LOEF

class LOEF_drawer : public QWidget {
    Q_OBJECT
   public:
    explicit LOEF_drawer(QWidget *parent = nullptr);

   private:
    double dpmm_;
    std::vector<LOEF::fixed_charge> fixed_charges_;
    LOEF::state_charge_selected_ *charge_selected_;

    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

   signals:

   private slots:
};

#endif  // LOEF_DRAWER_HPP
