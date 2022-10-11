/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef KIRAN_CPANEL_AUDIO_CPANEL_AUDIO_WIDGET_H
#define KIRAN_CPANEL_AUDIO_CPANEL_AUDIO_WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelAudioWidget;
}
QT_END_NAMESPACE

class CPanelAudioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CPanelAudioWidget(QWidget *parent = nullptr);

    ~CPanelAudioWidget() override;

    void init();

private:
    Ui::CPanelAudioWidget *ui;
};

#endif  //KIRAN_CPANEL_AUDIO_CPANEL_AUDIO_WIDGET_H
