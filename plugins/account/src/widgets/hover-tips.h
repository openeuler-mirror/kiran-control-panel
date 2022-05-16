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
 
#ifndef KIRAN_ACCOUNT_MANAGER_HOVER_TIPS_H
#define KIRAN_ACCOUNT_MANAGER_HOVER_TIPS_H

#include <QMap>
#include <QWidget>

class QLabel;

//TODO:暂时只做了居中显示

class HoverTips : private QWidget
{
    Q_OBJECT
public:
    enum HoverTipsTypeEnum
    {
        HOVE_TIPS_SUC,
        HOVE_TIPS_INFO,
        HOVE_TIPS_WARNING,
        HOVE_TIPS_ERR
    };
    Q_ENUMS(HoverTipsEnum);

public:
    explicit HoverTips(QWidget *parent = nullptr);
    ~HoverTips();

    void setTimeout(quint32 ms);

    void setIcon(HoverTipsTypeEnum typeEnum, const QString &icon);

    void show(HoverTipsTypeEnum typeEnum, const QString &msg);
    void hide();

private:
    void initUI();
    void updatePostion();
    void startHideTimer();
    void stopHideTimer();

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<HoverTipsTypeEnum, QString> m_tipsTypeIconMap = {
        {HOVE_TIPS_SUC, ":/kcp-account-images/suc.svg"},
        {HOVE_TIPS_INFO, ":/kcp-account-images/info.svg"},
        {HOVE_TIPS_WARNING, ":/kcp-account-images/warning.svg"},
        {HOVE_TIPS_ERR, ":/kcp-account-images/err.svg"}};
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    quint32 m_hideTimeout = 3000;
    int m_hideTimerID = -1;
};

#endif  //KIRAN_ACCOUNT_MANAGER_HOVER_TIPS_H
