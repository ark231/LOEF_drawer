#ifndef LOEF_DRAWER_QT_TOML_SETTINGS
#define LOEF_DRAWER_QT_TOML_SETTINGS
#include <QIODevice>
#include <QSettings>
namespace LOEF {
namespace qt {
inline QSettings::Format TOML_format = QSettings::InvalidFormat;
bool read_toml(QIODevice &, QSettings::SettingsMap &);
bool write_toml(QIODevice &, const QSettings::SettingsMap &);
}  // namespace qt
}  // namespace LOEF
#endif
