#include "qt_toml_settings.hpp"

#include <QDebug>
#include <QList>
#include <QLocale>
#include <QRegularExpression>
#include <QTextStream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <toml.hpp>
namespace LOEF {
namespace qt {
void decode_data(QSettings::SettingsMap &dst, const toml::value &data, const QString key_offset) {
    switch (data.type()) {
        case toml::value_t::boolean:
            dst[key_offset] = QVariant::fromValue(data.as_boolean());
            break;
        case toml::value_t::integer:
            dst[key_offset] = QVariant::fromValue(data.as_integer());
            break;
        case toml::value_t::floating:
            dst[key_offset] = QVariant::fromValue(data.as_floating());
            break;
        case toml::value_t::string: {
            QRegularExpression re(R"(@([a-zA-Z]+)\(([^)]+)\))");
            auto str = QString::fromStdString(data.as_string());
            auto matched = re.match(str);
            if (matched.hasMatch()) {
                if (matched.captured(1) == "QLocale") {
                    dst[key_offset] = QVariant::fromValue(QLocale(matched.captured(2)));
                } else {
                    std::stringstream err_msg;
                    err_msg << matched.captured(1).toStdString() << " is not supported";
                    throw std::runtime_error(err_msg.str());
                }
            } else {
                dst[key_offset] = QVariant::fromValue(str);
            }
        } break;
        case toml::value_t::array: {
            auto data_array = data.as_array();
            for (size_t i = 0; i < data_array.size(); i++) {
                decode_data(dst, data_array[i], QString("%1/%2").arg(key_offset).arg(i));
            }
            break;
        }
        case toml::value_t::table:
            for (const auto &[key_std, value] : data.as_table()) {
                auto key = QString::fromStdString(key_std);
                decode_data(dst, value, QString("%1/%2").arg(key_offset).arg(key));
            }
            break;
        default:
            std::stringstream err_msg;
            err_msg << data.type() << " is not supported";
            throw std::invalid_argument(err_msg.str());
    }
}
bool read_toml(QIODevice &device, QSettings::SettingsMap &map) {
    std::istringstream input_str(device.readAll().toStdString());
    auto parsed = toml::parse(input_str);
    for (const auto &[key_std, value] : parsed.as_table()) {
        auto key = QString::fromStdString(key_std);
        switch (value.type()) {
            case toml::value_t::boolean:
                map[key] = QVariant::fromValue(value.as_boolean());
                break;
            case toml::value_t::integer:
                map[key] = QVariant::fromValue(value.as_integer());
                break;
            case toml::value_t::floating:
                map[key] = QVariant::fromValue(value.as_floating());
                break;
            case toml::value_t::string:
                map[key] = QVariant::fromValue(QString::fromStdString(value.as_string()));
                break;
            case toml::value_t::array:
            case toml::value_t::table:
                decode_data(map, value, key);
                break;
            default:
                break;
        }
    }
    return true;
}
void assign_variant(toml::value &table, const std::string &key, const QVariant &value) {
    switch (value.type()) {
        case QVariant::Bool:
            table[key] = value.value<bool>();
            break;
        case QVariant::Int:
            table[key] = value.value<int>();
            break;
        case QVariant::Double:
            table[key] = value.value<double>();
            break;
        case QVariant::String:
            table[key] = value.value<QString>().toStdString();
            break;
        case QVariant::Locale:
            table[key] = QString("@QLocale(%1)").arg(value.value<QLocale>().bcp47Name()).toStdString();
            break;
        default:
            std::stringstream err_msg;
            err_msg << value.typeName() << " is not supported";
            throw std::invalid_argument(err_msg.str());
    }
}
void assign_recursive(toml::value &table, QStringList keys, QVariant &value) {
    if (keys.count() == 1) {
        assign_variant(table, keys.front().toStdString(), value);
    } else {
        auto key = keys.front().toStdString();
        keys.pop_front();
        assign_recursive(table[key], keys, value);
    }
}
bool write_toml(QIODevice &device, const QSettings::SettingsMap &map) {
    toml::value result;
    for (auto map_itr = map.keyValueBegin(); map_itr != map.keyValueEnd(); map_itr++) {
        auto key = map_itr->first;
        auto value = map_itr->second;
        auto sections = key.split("/");
        assign_recursive(result, sections, value);
    }
    device.write(toml::format(result, 0).data());
    return true;
}
}  // namespace qt
}  // namespace LOEF
