#include "mainwindow.hpp"

#include <QString>

#include "./ui_mainwindow.h"
#include "general_consts.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->label_app_info->setText(QStringLiteral("%1 \nv%2.%3.%4")
                                    .arg(LOEF::application_name)
                                    .arg(LOEF::version_major)
                                    .arg(LOEF::version_minor)
                                    .arg(LOEF::version_patch));
    this->setWindowTitle(LOEF::application_name);
}

MainWindow::~MainWindow() { delete ui; }
