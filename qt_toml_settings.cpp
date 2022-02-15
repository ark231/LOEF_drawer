#include "qt_toml_settings.hpp"

#include <QDebug>
#include <QList>
#include <QLocale>
#include <QRegularExpression>
#include <QTextStream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#define TOML11_PRESERVE_COMMENTS_BY_DEFAULT
#include <toml.hpp>
namespace LOEF {
namespace qt {
constexpr const char *LOEF_TOML_COMMENT_SECTIONNAME = "__LOEF_toml_comment__";
void store_comment(QSettings::SettingsMap &dst, const toml::value &data, const QString key_offset) {
    size_t i = 0;
    QString type = "";
    switch (data.type()) {
        case toml::value_t::boolean:
        case toml::value_t::integer:
        case toml::value_t::floating:
        case toml::value_t::string:
            type = "primitive";
            break;
        default:
            type = "structure";
            break;
    }
    for (const auto &comment : data.comments()) {
        dst[QString("%1/%2/%3/_%4").arg(LOEF_TOML_COMMENT_SECTIONNAME).arg(type).arg(key_offset).arg(i)] =
            QVariant::fromValue(QString::fromStdString(comment));
        i++;
    }
}
void decode_data(QSettings::SettingsMap &dst, const toml::value &data, const QString key_offset) {
    switch (data.type()) {
        case toml::value_t::boolean:
            dst[key_offset] = QVariant::fromValue(data.as_boolean());
            store_comment(dst, data, key_offset);
            break;
        case toml::value_t::integer:
            dst[key_offset] = QVariant::fromValue(data.as_integer());
            store_comment(dst, data, key_offset);
            break;
        case toml::value_t::floating:
            dst[key_offset] = QVariant::fromValue(data.as_floating());
            store_comment(dst, data, key_offset);
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
            store_comment(dst, data, key_offset);
        } break;
        case toml::value_t::array: {
            auto data_array = data.as_array();
            for (size_t i = 0; i < data_array.size(); i++) {
                decode_data(dst, data_array[i], QString("%1/%2").arg(key_offset).arg(i));
            }
            store_comment(dst, data, key_offset);
            break;
        }
        case toml::value_t::table:
            for (const auto &[key_std, value] : data.as_table()) {
                auto key = QString::fromStdString(key_std);
                decode_data(dst, value, QString("%1/%2").arg(key_offset).arg(key));
            }
            store_comment(dst, data, key_offset);
            break;
        default:
            std::stringstream err_msg;
            err_msg << data.type() << " is not supported";
            throw std::invalid_argument(err_msg.str());
    }
}
bool read_toml(QIODevice &device, QSettings::SettingsMap &map) {
    std::istringstream input_str(device.readAll().toStdString());
    auto parsed = toml::parse<toml::preserve_comments>(input_str);
    for (const auto &[key_std, value] : parsed.as_table()) {
        auto key = QString::fromStdString(key_std);
        decode_data(map, value, key);
    }
    return true;
}
void assign_variant(toml::value &table, const std::string &key, const QVariant &value,
                    const std::vector<std::string> comments) {
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
    for (const auto &comment : comments) {
        table[key].comments().push_back(comment);
    }
}
void assign_recursive(toml::value &table, QStringList keys, QVariant &value, std::vector<std::string> comments) {
    if (keys.count() == 1) {
        assign_variant(table, keys.front().toStdString(), value, comments);
    } else {
        auto key = keys.front().toStdString();
        keys.pop_front();
        assign_recursive(table[key], keys, value, comments);
    }
}
std::vector<std::string> retrive_comment(const QSettings::SettingsMap &map, QString &key, QVariant &value) {
    std::vector<std::string> result;
    QString type = "";
    switch (value.type()) {
        case QVariant::Bool:
        case QVariant::Int:
        case QVariant::Double:
        case QVariant::String:
        case QVariant::Locale:
            type = "primitive";
            break;
        default:
            type = "structure";
            break;
    }
    auto comment_key = QString("%1/%2/%3").arg(LOEF_TOML_COMMENT_SECTIONNAME).arg(type).arg(key);
    for (size_t i = 0;; i++) {
        auto comment_idx_key = QString("%1/_%2").arg(comment_key).arg(i);
        if (map.contains(comment_idx_key)) {
            result.push_back(map[comment_idx_key].value<QString>().toStdString());
        } else {
            break;
        }
    }
    return result;
}
bool write_toml(QIODevice &device, const QSettings::SettingsMap &map) {
    toml::value result;
    for (auto map_itr = map.keyValueBegin(); map_itr != map.keyValueEnd(); map_itr++) {
        auto key = map_itr->first;
        auto value = map_itr->second;
        auto sections = key.split("/");
        if (sections[0] == LOEF_TOML_COMMENT_SECTIONNAME) {
            continue;
        }
        auto comments = retrive_comment(map, key, value);
        assign_recursive(result, sections, value, comments);
    }
    device.write(toml::format(result, 0).data());
    return true;
}
}  // namespace qt
}  // namespace LOEF
