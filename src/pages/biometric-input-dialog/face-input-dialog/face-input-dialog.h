//
// Created by lxh on 2021/3/12.
//

#ifndef KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H
#define KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H

#include <QWidget>
#include <kiran-titlebar-window.h>

QT_BEGIN_NAMESPACE
namespace Ui { class FaceInputDialog; }
QT_END_NAMESPACE

class FaceInputDialog : public KiranTitlebarWindow {
Q_OBJECT

public:
    explicit FaceInputDialog(QWidget *parent = nullptr);
    ~FaceInputDialog() override;

private:
    void initUI();

Q_SIGNALS:
    void sigClose();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::FaceInputDialog *ui;
};

#endif //KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H
