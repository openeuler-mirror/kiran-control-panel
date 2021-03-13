//
// Created by lxh on 2021/3/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_face-input-dialog.h" resolved

#include "face-input-dialog.h"
#include "ui_face-input-dialog.h"

FaceInputDialog::FaceInputDialog(QWidget *parent) :
        KiranTitlebarWindow(),
        ui(new Ui::FaceInputDialog) {
    ui->setupUi(getWindowContentWidget());
    initUI();
}

FaceInputDialog::~FaceInputDialog() {
    delete ui;
}

void FaceInputDialog::initUI() {
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

void FaceInputDialog::closeEvent(QCloseEvent *event) {
    emit sigClose();
    QWidget::closeEvent(event);
}
