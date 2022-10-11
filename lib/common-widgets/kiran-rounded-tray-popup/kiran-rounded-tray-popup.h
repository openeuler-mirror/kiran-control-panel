/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H
#define KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H

#include <QWidget>

class QLayout;
class KiranRoundedTrayPopup:public QWidget
{
    Q_OBJECT
public:
    explicit KiranRoundedTrayPopup(QWidget* parent = nullptr);
    ~KiranRoundedTrayPopup();

    void setContentWidget(QWidget* widget);
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateContentMask(QWidget* widget);
    void init();

private:
    QLayout* m_contentLayout;
    QWidget* m_contentWidget=nullptr;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H
