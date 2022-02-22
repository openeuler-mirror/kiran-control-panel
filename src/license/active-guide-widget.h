/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */
#ifndef ACTIVEGUIDEWIDGET_H
#define ACTIVEGUIDEWIDGET_H

#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "active-list-item.h"
#include "config/config.h"
#include "load-animation-widget.h"
#include "show-qrcode.h"
namespace Ui
{
class ActGuideWidget;
}

/**
 * @brief  ActGuideWidget 激活向导窗口
 */
#ifdef DISABLE_KIRANWIDGETS
class ActGuideWidget : public QWidget
#else
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
class ActGuideWidget : public KiranTitlebarWindow
#endif
{
    Q_OBJECT

public:
    enum
    {
        Page_Start,
        Page_Manual,
        Page_Key,
        Page_Online,
        Page_Activating,
        Page_Successful,
        Page_Failed
    };
    explicit ActGuideWidget(QWidget* parent = NULL);
    ~ActGuideWidget();
    bool getLineEditStatus();

private:
    void iniUI();
    ActListItem* createActiveItem(const QString& text);
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
    bool eventFilter(QObject* target, QEvent* e);

private:
    Ui::ActGuideWidget* ui;
    ShowQRCode* showQRCode;
    LoadAnimationWidget* loadAnimation;
    ActListItem* listItemMode;
    ActListItem* listItemActive;
    ActListItem* listItemComplete;
    QString mc_code;
    bool activedFlag;
    bool byManual;
    bool byKey;
    bool byOnline;
};

#endif  // ACTIVEGUIDEWIDGET_H
