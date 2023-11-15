/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#pragma once

#include <QObject>

// FIXME: 只适配了KWIN 5.15.4中打入补丁才能支持X11色温功能(#68397)。
// 一些特定的KWIN版本失效,暂时未对其他KWIN版本做适配
class KWinColorCorrect: public QObject
{
    Q_OBJECT
public:
    enum Mode
    {
        MODE_AUTOMATIC,
        MODE_MANUAL
    };
    KWinColorCorrect(QObject* parent = nullptr);
    ~KWinColorCorrect();

    /// @brief 是否可用
    /// @return true/可用 , false/不可用(缺少kwin或版本不兼容)
    bool isValid() const;
    
    /// @brief 设置自动模式,自动根据时间调整色温
    /// @return 是否成功
    bool setAutomaticMode();

    /// @brief 设置手动模式
    /// @param percent 0(冷色)~1(暖色)
    /// @return 是否成功
    bool setManualMode(qreal percent);

    KWinColorCorrect::Mode getMode();
    qreal getPercent();
    
private:
    /// @brief 将具体色温设置项转化成百分比
    /// @param temperature 具体色温
    /// @return 0(冷)~1(暖)之间的浮点数
    qreal colorTemperatureToPencent(int temperature);

    /// @brief 百分比转换成具体色温值
    /// @param percent 0(冷)~1(暖)之间的浮点数
    /// @return 具体色温
    int percentToColorTemperature(qreal percent);

private:
    bool m_isValid = false;    
    Mode m_mode = MODE_MANUAL;
    qreal m_percent = 0.5;
};