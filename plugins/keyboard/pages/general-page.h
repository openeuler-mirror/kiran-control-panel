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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef GENERALPAGE_H
#define GENERALPAGE_H

#include <QTimer>
#include <QWidget>

namespace Ui
{
class GeneralPage;
}

class KeyboardBackEndProxy;
class GeneralPage : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralPage(QWidget *parent = 0);
    ~GeneralPage();
    QSize sizeHint() const override;

private:
    void init();

private slots:
    void handleSaverTimerTimeOut();
    void handleSwitchRepeatKeyToggled(bool checked);
    void handleSwitchCapsLockTipToggled(bool checked);
    void handleSwitchNumLockTipsToggled(bool checked);

private:
    Ui::GeneralPage *ui;
    KeyboardBackEndProxy *m_keyboardInterface;
    QTimer *m_timer = nullptr;
    bool m_repeateEnabled = false;
    qint32 m_delay;
    qint32 m_interval;
    bool m_modifierLockEnabled;
    bool m_capslockTipsEnabled;
    bool m_numlockTipsEnabled;
};

#endif  // GENERALPAGE_H
