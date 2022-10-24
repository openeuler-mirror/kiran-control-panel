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

#ifndef FONTS_H
#define FONTS_H

#include <QComboBox>
#include <QWidget>

namespace Ui
{
class Fonts;
}

class Fonts : public QWidget
{
    Q_OBJECT

public:
    explicit Fonts(QWidget *parent = 0);
    ~Fonts();

    QSize sizeHint() const override;
private:
    bool initUI();
    void getCurrentFontInfo(int fontType);
    void setFont(int fontType, QStringList fontInfoList);
    void connectSignals();
    void showFontInfo(QComboBox *nameParent, QComboBox *sizeParent, QString name, QString size);

public slots:
    void handleFontChanged(int type, QString fontInfo);

private:
    Ui::Fonts *ui;
    QStringList m_applicationFontInfo;
    QStringList m_windowTitleFontInfo;
    QStringList m_monospaceFontInfo;
    QStringList m_fontFamilies;
};

#endif  // FONTS_H
