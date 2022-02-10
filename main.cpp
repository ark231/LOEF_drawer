#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <QTranslator>

#include "general_consts.hpp"
#include "mainwindow.hpp"
#include "qt_consts.hpp"
#include "qt_message_handler.hpp"
#include "qt_toml_settings.hpp"

void init_global_vars(QSettings &);
void generate_settings_file(QSettings &);
int main(int argc, char *argv[]) {
    int exit_code;
    do {
        qInstallMessageHandler(LOEF::qt::message_hander);
        QApplication a(argc, argv);
        LOEF::qt::TOML_format = QSettings::registerFormat("toml", LOEF::qt::read_toml, LOEF::qt::write_toml);

        Q_ASSERT(LOEF::qt::TOML_format != QSettings::InvalidFormat);
        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings." + LOEF::configfile_ext,
                           LOEF_QT_CONFIG_FORMAT);
        settings.beginGroup("init");
        QLocale selected_locale = settings.value("locale", QLocale("en")).value<QLocale>();
        settings.endGroup();
        init_global_vars(settings);
        if (!QFile(settings.fileName()).exists()) {
            generate_settings_file(settings);
        }
        QTranslator translator;
        translator.load(selected_locale, "LOEF_drawer", "_", QCoreApplication::applicationDirPath() + "/translations");
        a.installTranslator(&translator);
        MainWindow w(selected_locale);
        w.show();
        exit_code = a.exec();
    } while (exit_code == LOEF::restart_code);
    return exit_code;
}
void init_global_vars(QSettings &settings) {
    settings.beginGroup("draw");
    settings.beginGroup("radius");
    LOEF::radius::FIXED = settings.value("FIXED", 2.5).value<double>() * LOEF::millimetre;
    LOEF::default_val::radius::FIXED = LOEF::radius::FIXED;
    settings.endGroup();
    LOEF::ARROW_HEIGHT = settings.value("ARROW_HEIGHT", 3.0).value<double>() * LOEF::millimetre;
    LOEF::initial_fixed_pos_x = settings.value("initial_fixed_pos_x", 50.0).value<double>() * LOEF::millimetre;
    LOEF::initial_fixed_pos_y = settings.value("initial_fixed_pos_y", 50.0).value<double>() * LOEF::millimetre;
    LOEF::initial_fixed_charge =
        settings.value("initial_fixed_charge", 0.0).value<double>() * LOEF::boostunits::coulomb;
    LOEF::initial_inverse_permittivity =
        settings.value("initial_inverse_permittivity", 12.0).value<double>() * LOEF::inverse_permittivity_unit_quantity;
    LOEF::interval_steps = settings.value("interval_steps", 0.5).value<double>() * LOEF::millimetre;
    settings.endGroup();
}
void generate_settings_file(QSettings &settings) {
    settings.beginGroup("draw");
    settings.beginGroup("radius");
    settings.setValue("FIXED", 2.5);
    settings.endGroup();
    settings.setValue("ARROW_HEIGHT", 3.0);
    settings.setValue("initial_fixed_pos_x", 50.0);
    settings.setValue("initial_fixed_pos_y", 50.0);
    settings.setValue("initial_fixed_charge", 0.0);
    settings.setValue("initial_inverse_permittivity", 12.0);
    settings.setValue("interval_steps", 0.5);
    settings.endGroup();
    settings.sync();
}
