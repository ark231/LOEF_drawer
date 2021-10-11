#include <QApplication>
#include <QCoreApplication>
#include <QSettings>
#include <QTranslator>

#include "mainwindow.hpp"
#include "qt_consts.hpp"

int main(int argc, char *argv[]) {
    int exit_code;
    do {
        QApplication a(argc, argv);

        QSettings settings(QCoreApplication::applicationDirPath() + "/settings/settings.ini", QSettings::IniFormat);
        settings.beginGroup("init");
        QLocale selected_locale = settings.value("locale", QLocale("en")).value<QLocale>();
        settings.endGroup();
        QTranslator translator;
        translator.load(selected_locale, "LOEF_drawer", "_", QCoreApplication::applicationDirPath() + "/translations");
        a.installTranslator(&translator);
        MainWindow w(selected_locale);
        w.show();
        exit_code = a.exec();
    } while (exit_code == LOEF::restart_code);
    return exit_code;
}
