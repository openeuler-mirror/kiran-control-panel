/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include <QWidget>

namespace Ui
{
class ScreensaverPage;
}

class QGSettings;
class KiranSwitchButton;
class ScreensaverPage : public QWidget
{
    Q_OBJECT
public:
    explicit ScreensaverPage(QWidget *parent = nullptr);
    ~ScreensaverPage();

    static bool isSupported();
    static QWidget* createPage();
    
private:
    void initUI();
    void init();
    bool checkThemeAvaiable(const QString& themeID);

private slots:
    void processSettingsChanged(const QString& key);
    void processSwitcherTrigger();
    void processcurrentThemeChanged();
    
private:
    Ui::ScreensaverPage *ui;
    KiranSwitchButton* m_idleActivteScreenSaverSwitch;
    QGSettings* m_gSettings;
};