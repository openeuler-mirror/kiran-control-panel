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
#include "kwin-color-correct.h"
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#define KWIN_COLOR_CORRECT_SERVICE_NAME "org.kde.KWin"
#define KWIN_COLOR_CORRECT_SERVICE_PATH "/"
#define KWIN_COLOR_CORRECT_SERVICE_INTERFACE "org.kde.kwin.ColorCorrect"

#define KWIN_COLOR_CORRECT_SERVICE_GET_NIGHTCOLORINFO_METHOD "nightColorInfo"
#define KWIN_COLOR_CORRECT_SERVICE_SET_NIGHTCOLORINFO_METHOD "setNightColorConfig"

enum NightColorMode
{
    // timings are based on provided location data
    Automatic = 0,
    // timings are based on fixed location data
    Location,
    // fixed timings
    Timings,
    // 手动指定色温
    Constant
};

KWinColorCorrect::KWinColorCorrect(QObject* parent)
    : QObject(parent)
{
    auto isServiceRegistered = QDBusConnection::sessionBus().interface()->isServiceRegistered(KWIN_COLOR_CORRECT_SERVICE_NAME);
    m_isValid = isServiceRegistered.value();

    if (m_isValid)
    {
        auto getNightColorInfoMethod = QDBusMessage::createMethodCall(KWIN_COLOR_CORRECT_SERVICE_NAME,
                                                                      "/ColorCorrect",
                                                                      KWIN_COLOR_CORRECT_SERVICE_INTERFACE,
                                                                      KWIN_COLOR_CORRECT_SERVICE_GET_NIGHTCOLORINFO_METHOD);

        auto reply = QDBusConnection::sessionBus().call(getNightColorInfoMethod);

        QList<QVariant> arguments = reply.arguments();
        QVariantMap varMap = qdbus_cast<QVariantMap>(arguments.at(0).value<QDBusArgument>());

        int mode = varMap["Mode"].toInt();
        if (mode == NightColorMode::Constant)
        {
            int currentColorTemperature = varMap["CurrentColorTemperature"].toInt();

            m_mode = MODE_MANUAL;
            m_percent = colorTemperatureToPencent(currentColorTemperature);
        }
        else
        {
            m_mode = MODE_AUTOMATIC;
        }
    }
}

KWinColorCorrect::~KWinColorCorrect()
{
}

bool KWinColorCorrect::isValid() const
{
    return m_isValid;
}

bool KWinColorCorrect::setAutomaticMode()
{
    if (!m_isValid)
    {
        KLOG_WARNING() << "set automatic mode error:KWin is missing or currently not supported by KWin";
        return false;
    }

    QVariantMap args = {
        {"Active",true},
        {"Mode", (int)NightColorMode::Timings},
        {"MorningBeginFixed", "06:00:00"},
        {"EveningBeginFixed", "18:00:00"},
    };
    auto setNightColorConfigMethod = QDBusMessage::createMethodCall(KWIN_COLOR_CORRECT_SERVICE_NAME,
                                                                    "/ColorCorrect",
                                                                    KWIN_COLOR_CORRECT_SERVICE_INTERFACE,
                                                                    KWIN_COLOR_CORRECT_SERVICE_SET_NIGHTCOLORINFO_METHOD);
    setNightColorConfigMethod << args;
    auto reply = QDBusConnection::sessionBus().call(setNightColorConfigMethod);

    bool isSuccess = false;
    if (reply.arguments().size() > 0)
    {
        isSuccess = reply.arguments().first().toBool();
    }

    if (!isSuccess)
    {
        KLOG_WARNING() << "set authmatic mode error:" << args << reply.errorMessage();
        return false;
    }

    m_mode = MODE_AUTOMATIC;
    m_percent = 0.0;
    return true;
}

bool KWinColorCorrect::setManualMode(qreal percent)
{
    if (!m_isValid)
    {
        KLOG_WARNING() << "set manual mode error:KWin is missing or currently not supported by KWin";
        return false;
    }

    int colorTemperature = percentToColorTemperature(percent);

    QVariantMap args = {
        {"Active",true},
        {"Mode", (int)NightColorMode::Constant},
        {"NightTemperature", colorTemperature}};
    auto setNightColorConfigMethod = QDBusMessage::createMethodCall(KWIN_COLOR_CORRECT_SERVICE_NAME,
                                                                    "/ColorCorrect",
                                                                    KWIN_COLOR_CORRECT_SERVICE_INTERFACE,
                                                                    KWIN_COLOR_CORRECT_SERVICE_SET_NIGHTCOLORINFO_METHOD);
    setNightColorConfigMethod << args;
    auto reply = QDBusConnection::sessionBus().call(setNightColorConfigMethod);

    bool isSuccess = false;
    if (reply.arguments().size() > 0)
    {
        isSuccess = reply.arguments().first().toBool();
    }

    if (!isSuccess)
    {
        KLOG_WARNING() << "set manual mode error:" << args << reply.errorMessage();
        return false;
    }

    m_mode = MODE_MANUAL;
    m_percent = percent;
    return true;
}

KWinColorCorrect::Mode KWinColorCorrect::getMode()
{
    return m_mode;
}

qreal KWinColorCorrect::getPercent()
{
    return m_percent;
}

qreal getMappingRangeValue(qreal sourceValue, qreal sourceRangeMin, qreal sourceRangeMax,
                           qreal targetRangetMin, qreal targetRangeMax)
{
    return ( ( (targetRangeMax - targetRangetMin) / (sourceRangeMax - sourceRangeMin) ) * (sourceValue - sourceRangeMin) ) + targetRangetMin;
}

#define COLOR_TEMPERATURE_WARM_MAX 2500
#define COLOR_TEMPERATURE_STANDARD 5500
#define COLOR_TEMPERATURE_COLD_MAX 6500

// [warm(2500) - standard(5500) - cold(6500)] -> [1~0]
qreal KWinColorCorrect::colorTemperatureToPencent(int temperature)
{
    if (temperature > COLOR_TEMPERATURE_COLD_MAX)
    {
        temperature = COLOR_TEMPERATURE_COLD_MAX;
    }

    if (temperature < COLOR_TEMPERATURE_WARM_MAX)
    {
        temperature = COLOR_TEMPERATURE_WARM_MAX;
    }

    qreal percent = 0.0;
    if (temperature >= COLOR_TEMPERATURE_WARM_MAX && temperature < COLOR_TEMPERATURE_STANDARD)  // 翻转并且转换至 1~0.5 区间
    {
        // [COLOR_TEMPERATURE_WARM_MAX ~ COLOR_TEMPERATURE_STANDARD] 翻转
        temperature = qAbs(temperature - COLOR_TEMPERATURE_STANDARD);
        // 映射至[0.5 ~ 1]
        percent = getMappingRangeValue(temperature, 0, qAbs(COLOR_TEMPERATURE_WARM_MAX - COLOR_TEMPERATURE_STANDARD), 0.5, 1);
    }
    else if (temperature == COLOR_TEMPERATURE_STANDARD)  // standard 5500
    {
        percent = 0.5;
    }
    else  // 翻转并且转换至 0~0.5 区间
    {
        // [COLOR_TEMPERATURE_STANDARD ~ COLOR_TEMPERATURE_COLD_MAX] 翻转
        temperature = qAbs(temperature - COLOR_TEMPERATURE_COLD_MAX);
        // 映射[0 ~ 0.5]
        percent = getMappingRangeValue(temperature, 0, qAbs(COLOR_TEMPERATURE_STANDARD - COLOR_TEMPERATURE_COLD_MAX), 0, 0.5);
    }

    KLOG_DEBUG("color temperature:%dk -> %.0lf%%", temperature,percent*100);
    return percent;
}

int KWinColorCorrect::percentToColorTemperature(qreal percent)
{
    if (percent > 1.0)
    {
        percent = 1.0;
    }

    if (percent < 0.0)
    {
        percent = 0.0;
    }

    double colorTemperature = COLOR_TEMPERATURE_STANDARD;
    if (percent > 0.5)
    {
        //暖色 0.5~1 - 5500-2500k
        auto colorTemperatureDiff = getMappingRangeValue(percent, 0.5, 1, 0, COLOR_TEMPERATURE_STANDARD - COLOR_TEMPERATURE_WARM_MAX);
        colorTemperature -= colorTemperatureDiff;
    }
    else if (qFuzzyCompare(percent, 0.5))
    {
        colorTemperature = COLOR_TEMPERATURE_STANDARD;
    }
    else
    {
        //冷色 0~0.5 - 6500-5500k
        auto colorTemperatureDiff = getMappingRangeValue(percent, 0, 0.5, 0, COLOR_TEMPERATURE_COLD_MAX - COLOR_TEMPERATURE_STANDARD);
        colorTemperature =  COLOR_TEMPERATURE_COLD_MAX - colorTemperatureDiff;
    }

    KLOG_DEBUG("color temperature: %.1lf%% -> %.0lfk",percent*100.0,colorTemperature);
    return colorTemperature;
}