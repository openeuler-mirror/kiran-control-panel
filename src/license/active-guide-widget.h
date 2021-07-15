/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
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
#ifndef ACTIVEGUIDEWIDGET_H
#define ACTIVEGUIDEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include "show-qrcode.h"
#include "active-list-item.h"
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include "load-animation-widget.h"
namespace Ui {
class ActGuideWidget;
}

/**
 * @brief  ActGuideWidget 激活向导窗口
 */
class ActGuideWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    enum {Page_Start, Page_Manual,Page_Key,Page_Online,Page_Activating,Page_Successful,Page_Failed};
    explicit ActGuideWidget(QWidget* parent=nullptr);
    ~ActGuideWidget();
    bool getLineEditStatus();

private:
    void iniUI();
    ActListItem* createActiveItem(const QString &text);
    bool getMachineCode();
    void sleep(int);
    void hideLabelPointsDefault();
    void setLabelPointsVisible(bool);
    void getJsonValueFromString(const QString jsonString);

signals:
    void systemIsActived(bool actived);
    void sigShowqrcodewgt(QPoint oPoint);

public slots:
    //void get_page_index();
    void handleNextButton();
    void handleActiveButton();
    void handleBackButton();
    void handleCloseButton();
    void getLicenseStatus(bool isRegister);
    void popupQRCode(QPoint oPoint);
    void setManualLineEditStatus();
    void setKeyLineEditStatus();

protected:
    bool eventFilter(QObject *target, QEvent *e);

private:
    Ui::ActGuideWidget *ui;
    ShowQRCode * showQRCode;
    LoadAnimationWidget *loadAnimation;
    ActListItem* listItemMode;
    ActListItem* listItemActive;
    ActListItem* listItemComplete;
    QString mc_code;
    bool activedFlag;
    bool byManual;
    bool byKey;
    bool byOnline;
};

#endif // ACTIVEGUIDEWIDGET_H
