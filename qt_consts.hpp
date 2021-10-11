#ifndef LOEF_DRAWER_QT_CONSTS
#define LOEF_DRAWER_QT_CONSTS
#include <Qt>

namespace LOEF {
enum class item_data_role { id_role = Qt::UserRole };
constexpr int restart_code = -0x8efcfa;  //ロジバンの{refcfa}より(x1はx2回目に再起動する)
}  // namespace LOEF
#endif
