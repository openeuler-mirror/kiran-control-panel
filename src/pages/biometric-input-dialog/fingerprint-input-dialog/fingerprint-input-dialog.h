//
// Created by lxh on 2021/3/12.
//

#ifndef KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
#define KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H

#include <kiran-titlebar-window.h>
#include <QThread>
#include <QWidget>
#include "fingerprint-input-worker.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class FingerprintInputDialog;
}
QT_END_NAMESPACE

class FingerprintInputDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit FingerprintInputDialog(QWidget *parent = nullptr);
    ~FingerprintInputDialog() override;

    QString getFingerDataID();

Q_SIGNALS:
    void sigClose();

private:
    void init();
    enum TipType
    {
        TIP_TYPE_INFO,
        TIP_TYPE_ERROR
    };
    void setTips(TipType type, const QString &tip);

private slots:
    void slotShowStatus(unsigned int progress, const QString &msg);
    void slotEnrollComplete(bool isSuccess, const QString &msg, const QString &id);
    void slotEnrollError(const QString &errMsg);

private:
    void setProgress(unsigned int value);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::FingerprintInputDialog *ui;
    FingerprintInputWorker      m_worker;
    QString                     m_fingerDataID;
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
