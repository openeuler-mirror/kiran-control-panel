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

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QTimer>

namespace Ui
{
class Fonts;
}

class QComboBox;
class Fonts : public QWidget
{
    Q_OBJECT

public:
    explicit Fonts(QWidget *parent = 0);
    ~Fonts();
    QSize sizeHint() const override;
    static QWidget* createPage();
private:
    bool initUI();
    void initConnections();
    bool updateFontToBackend(int fontType, const QString& fontFamily,int fontSize);

private slots:
    void onCurrentFontFamilyChanged();
    void onSliderValueChanged(int value);
    void updateUiCurrentFontFamily(QComboBox* combo);
    void updateAllFontWordSize();
    void onBackendFontChanged(int type, const QString& fontFamily,int fontSize);
    void resetFontSettings();

private:
    Ui::Fonts *ui;
    QMap<QComboBox*,QList<int>> m_comboFontTypesMap;
    QTimer m_updateFontSizeTimer;
    // QMap<字体类型, <字体类型名称ComboBox,字体类型字号ComboBox>>
    QMap<int,QPair<QComboBox*,QComboBox*>> m_fontTypeComboBoxMap;
    static QStringList fontSizes;
};

#endif  // FONTS_H
