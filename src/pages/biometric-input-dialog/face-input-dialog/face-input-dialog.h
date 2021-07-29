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
 
#ifndef KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H
#define KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H

#include <kiran-titlebar-window.h>
#include <QThread>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class FaceInputDialog;
}
QT_END_NAMESPACE

class BiometricsInterface;
class FaceEnrollWorker;

class FaceInputDialog : public KiranTitlebarWindow
{
    Q_OBJECT
    enum TipType
    {
        TIP_TYPE_INFO,
        TIP_TYPE_ERROR
    };

public:
    explicit FaceInputDialog(QWidget *parent = nullptr);
    ~FaceInputDialog() override;

    QString getFaceDataID();

private:
    void init();
    void initUI();
    bool startEnroll();
    void stopEnroll();
    void generateNewPreviewImage();
    void setTips(FaceInputDialog::TipType type, const QString &tip);

Q_SIGNALS:
    void sigClose();

protected slots:
    void slotHasNewImage(QImage image);
    void slotFaceAxis(QList<QRect> rect);
    void slotUpdateEnrollFaceStatus(const QString &message, const QString &id, int progress, bool done);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::FaceInputDialog *ui;
    BiometricsInterface *m_interface;
    FaceEnrollWorker *m_enrollThread;
    QImage m_tempImage;
    QList<QRect> m_faces;
    bool m_enrollStarted = false;  ///标志是否已经开始采集，根据标志判断是否接受DBus发来的进度信号
    QString m_biometricID;
    bool m_isSave = false;
    QTimer m_refeshDelayTimer;
};

#endif  //KIRAN_ACCOUNT_MANAGER_FACE_INPUT_DIALOG_H
