 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 

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
    FingerprintInputWorker m_worker;
    BiometricsInterface *m_interface;
    QString m_fingerDataID;
    bool m_isSave = false;
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_DIALOG_H
