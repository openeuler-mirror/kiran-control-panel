//
// Created by lxh on 2021/3/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_fingerprint-input-dialog.h" resolved

#include "fingerprint-input-dialog.h"
#include "ui_fingerprint-input-dialog.h"

FingerprintInputDialog::FingerprintInputDialog(QWidget *parent) :
        KiranTitlebarWindow(),
        ui(new Ui::FingerprintInputDialog) {
    ui->setupUi(getWindowContentWidget());
    initUI();
}

FingerprintInputDialog::~FingerprintInputDialog() {
    delete ui;
}

void FingerprintInputDialog::initUI() {
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setTitle(tr("Finger Enroll"));
}

void FingerprintInputDialog::closeEvent(QCloseEvent *event) {
    emit sigClose();
    QWidget::closeEvent(event);
}
