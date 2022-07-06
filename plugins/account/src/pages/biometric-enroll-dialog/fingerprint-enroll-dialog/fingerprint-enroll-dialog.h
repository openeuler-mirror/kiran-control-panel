/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
#define KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H

#include <kiran-titlebar-window.h>
#include <QThread>
#include <QWidget>
#include "fingerprint-enroll-worker.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class FingerprintEnrollDialog;
}
QT_END_NAMESPACE

class KSDBiometricsProxy;
class FingerprintEnrollDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit FingerprintEnrollDialog(QWidget *parent = nullptr);
    ~FingerprintEnrollDialog() override;

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
    Ui::FingerprintEnrollDialog *ui;
    FingerprintInputWorker m_worker;
    KSDBiometricsProxy *m_interface;
    QString m_fingerDataID;
    bool m_isSave = false;
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
