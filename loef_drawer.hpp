#ifndef LOEF_DRAWER_HPP
#define LOEF_DRAWER_HPP

#include <QWidget>
#include <unordered_map>
#include <vector>

#include "charges.hpp"
#include "vec2d.hpp"
namespace LOEF {
class state_charge_selected_;
class id_handler;
}  // namespace LOEF
using id_type = size_t;
class LOEF_drawer : public QWidget {
    Q_OBJECT
   public:
    explicit LOEF_drawer(QWidget *parent = nullptr);

   private:
    LOEF::dot_per_millimetre_value dpmm_;
    std::unordered_map<id_type, LOEF::fixed_charge> fixed_charges_;
    LOEF::id_handler *fixed_charge_id_handler_;
    std::unordered_map<id_type, LOEF::charge_pen> charge_pens_;
    std::unordered_map<id_type, std::shared_ptr<LOEF::LOEF_path>> charge_paths_;
    LOEF::id_handler *charge_pen_id_handler_;
    LOEF::state_charge_selected_ *charge_selected_;

    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

   signals:

   private slots:
};

#endif  // LOEF_DRAWER_HPP
