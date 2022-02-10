#ifndef LOEF_DRAWER_QT_CONSTS
#define LOEF_DRAWER_QT_CONSTS
#include <QPoint>
#include <QStringView>
#include <Qt>

namespace LOEF {
enum class item_data_role { id_role = Qt::UserRole };
constexpr int restart_code = -0x8efcfa;
constexpr QPoint invalid_position = QPoint(-1, -1);  //画面上の座標はすべて正
constexpr const char* configfile_ext = "toml";
}  // namespace LOEF
#ifndef LOEF_QT_CONFIG_FORMAT
#    define LOEF_QT_CONFIG_FORMAT LOEF::qt::TOML_format
#else
#    error "macro name collision"
#endif
#endif
