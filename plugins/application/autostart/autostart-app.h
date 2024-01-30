/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#ifndef __AUTOSTART_APP_H__
#define __AUTOSTART_APP_H__

class QString;
class QIcon;
class AutostartApp
{
public:
    AutostartApp(QString autoStartAppFilePath);
    AutostartApp() = default;
    virtual ~AutostartApp();
    void parseAutoStartApp(const QString& filePath);

    QString getDesktopName();
    QString getName();
    QString getFilePath();
    bool getShown();
    bool getHidden();
    bool getNoDisplay();
    QIcon getIcon();

    void setAutoStartAppStatus(bool status);

private:
    QString m_desktopName;
    QString m_name;
    QString m_filePath;
    bool m_shown = false;
    bool m_hidden = false;
    bool m_noDisplay = false;
    QIcon m_icon;
};


#endif
