/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-timedate is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_
#define KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DateTimeSettings;
}
QT_END_NAMESPACE

class DateTimeSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeSettings(QWidget *parent = nullptr);
    ~DateTimeSettings() override;

    void reset();

private:
    void init();

private slots:
    void handleButtonSaveClicked();
    void handleButtonResetClicked();

private:
    Ui::DateTimeSettings *ui;
};

#endif  //KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_
