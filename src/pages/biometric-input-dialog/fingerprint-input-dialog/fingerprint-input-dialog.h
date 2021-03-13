//
// Created by lxh on 2021/3/12.
//

#ifndef KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
#define KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H

#include <QWidget>
#include <kiran-titlebar-window.h>

QT_BEGIN_NAMESPACE
namespace Ui { class FingerprintInputDialog; }
QT_END_NAMESPACE

class FingerprintInputDialog : public KiranTitlebarWindow {
Q_OBJECT

public:
    explicit FingerprintInputDialog(QWidget *parent = nullptr);
    ~FingerprintInputDialog() override;

Q_SIGNALS:
    void sigClose();

private:
    void initUI();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::FingerprintInputDialog *ui;
};

#endif //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
