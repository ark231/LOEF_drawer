#ifndef LOEF_DRAWER_QT_CONSTS
#define LOEF_DRAWER_QT_CONSTS
#include <QPoint>
#include <Qt>

namespace LOEF {
enum class item_data_role { id_role = Qt::UserRole };
constexpr int restart_code = -0x8efcfa;              //ロジバンの{refcfa}より(x1はx2回目に再起動する)
constexpr QPoint invalid_position = QPoint(-1, -1);  //画面上の座標はすべて正
}  // namespace LOEF
#endif
