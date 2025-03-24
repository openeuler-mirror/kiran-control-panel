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

// clang-format off
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QJsonObject>
#include "../theme-widget-group.h"
#include "../theme-widget.h"
#include "cursor-themes.h"
#include "dbus-interface/appearance-global-info.h"
#include <kiran-log/qt5-log-i.h>
#include <X11/Xcursor/Xcursor.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/appearance-i.h>
// clang-format on

static QStringList cursors{"left_ptr", "right_ptr", "top_left_corner", "top_right_corner", "size_hor", "pointer"};

// 兼容不同开源版本光标名称，若光标名不存在，则使用默认光标
static const std::vector<const char *> cursorNames[] = {
    {"left_ptr", "default", "top_left_arrow", "left_arrow"},
    {"pointing_hand", "pointer", "hand1", "e29285e634086352946a0e7090d73106", "default"},
    {"size_ver", "ns-resize", "v_double_arrow", "00008160000006810000408080010102", "default"},
    {"size_hor", "ew-resize", "h_double_arrow", "028006030e0e7ebffc7f7070c0600140", "default"},
    {"size_bdiag", "nesw-resize", "50585d75b494802d0151028115016902", "fcf1c3c7cd4491d801f1e1c78f100000", "default"},
    {"size_fdiag", "nwse-resize", "38c5dff7c7b8962045400281044508d2", "c7088f0f3e6c8088236ef8e1e3e70000", "default"}};

CursorThemes::CursorThemes(QWidget *parent) : QWidget(parent)
{
}

bool CursorThemes::initUI()
{
    if (!getCursorThemes(APPEARANCE_THEME_TYPE_CURSOR))
    {
        return false;
    }

    AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR, m_currentCursorTheme);
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(10);

    QLabel *label_text = new QLabel(this);
    label_text->setText(tr("Cursor Themes Settings"));

    QWidget *cursorWidget = createCursorWidget();

    mainVLayout->addWidget(label_text);
    mainVLayout->addWidget(cursorWidget);
    mainVLayout->addStretch();

    return true;
}

void CursorThemes::updateCursorTheme(QString newCursorTheme)
{
    QList<ThemeWidget *> widgets = m_themeWidgetGroup->getThemeWidgetList();
    foreach (ThemeWidget *widget, widgets)
    {
        if (widget->getTheme() == newCursorTheme)
        {
            widget->setSelectStatus(true, APPEARANCE_THEME_TYPE_CURSOR);
            m_currentCursorTheme = newCursorTheme;
            emit sigSetCursorTheme(true, newCursorTheme);
        }
        else
            widget->setSelectStatus(false, APPEARANCE_THEME_TYPE_CURSOR);
    }
}

/**
 * @brief CursorThemes::getCursorThemes 获取光标主题信息，包括名字和路径
 * @param Type 主题类型
 * @return true 成功
 *         false 失败
 */
bool CursorThemes::getCursorThemes(int Type)
{
    QString themeJson = nullptr;
    if (!AppearanceGlobalInfo::instance()->getAllThemes(Type, themeJson))
    {
        return false;
    }
    if (getJsonValueFromString(themeJson) <= 0)
    {
        KLOG_ERROR() << "Can't convert json string or there is no cursor themes!";
        return false;
    }
    return true;
}

int CursorThemes::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_ERROR() << " please check the string " << jsonString.toLocal8Bit().data();
        return -1;
    }
    if (jsonDocument.isArray())
    {
        QJsonArray array = jsonDocument.array();
        int iconThemesNum = array.size();
        if (iconThemesNum < 1)
            return -1;
        for (int i = 0; i < iconThemesNum; i++)
        {
            QJsonValue value = array.at(i);
            if (value.type() == QJsonValue::Object)
            {
                QJsonObject themeInfoObj = value.toObject();
                if (themeInfoObj.contains("name"))
                {
                    QJsonValue themeValue = themeInfoObj.value("name");
                    if (themeValue.isString())
                    {
                        QString name = themeValue.toVariant().toString();
                        m_cursorThemesName.insert(i, name);
                    }
                }
                if (themeInfoObj.contains("path"))
                {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if (themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        m_cursorThemesPath.insert(i, path);
                    }
                }
            }
        }
    }
    return m_cursorThemesName.size();
}

/**
 * @brief CursorThemes::createCursorWidget 创建光标控件
 * @return 返回创建好的控件
 */
QWidget *CursorThemes::createCursorWidget()
{
    m_themeWidgetGroup = new ThemeWidgetGroup(this);

    QWidget *cursorWidget = new QWidget(this);
    QVBoxLayout *mainVLayout = new QVBoxLayout(cursorWidget);
    mainVLayout->setSpacing(4);
    mainVLayout->setMargin(0);
    int size = 22;

    for (int i = 0; i < m_cursorThemesName.size(); i++)
    {
        QList<QPixmap> cursorMap;
        QDir cursorThemesDir(m_cursorThemesPath.at(i));

        for (const auto &cursors : cursorNames)
        {
            for (const QString &name : cursors)
            {
                QPixmap cursorPixmap = loadCursorImage(name, cursorThemesDir.dirName(), size);
                if (!cursorPixmap.isNull())
                {
                    cursorMap.append(cursorPixmap);
                    break;
                }
            }
        }

        if (cursorMap.isEmpty())
        {
            KLOG_WARNING() << "No valid cursors found for theme:" << m_cursorThemesPath.at(i);
            continue;
        }

        ThemeWidget *themeWidget = new ThemeWidget(QSize(size, size), m_currentCursorTheme,
                                                   m_cursorThemesName.at(i), cursorMap);
        mainVLayout->addWidget(themeWidget);
        m_themeWidgetGroup->addWidget(themeWidget);

        if (m_currentCursorTheme == m_cursorThemesName.at(i))
        {
            m_themeWidgetGroup->setCurrentWidget(themeWidget);
        }
        themeWidget->setTheme(m_cursorThemesName.at(i));
    }
    mainVLayout->addStretch();

    connect(m_themeWidgetGroup, &ThemeWidgetGroup::themeWidgetChange,
            [=](ThemeWidget *preWidget, ThemeWidget *currWidget)
            {
                if (preWidget)
                {
                    if (preWidget == currWidget)
                    {
                        emit sigSetCursorTheme(false, m_currentCursorTheme);
                        return;
                    }
                }
                if (AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_CURSOR,
                                                               currWidget->getTheme()))
                {
                    KLOG_INFO() << "set cursor theme successful";
                }
                else
                {
                    KiranMessageBox::message(nullptr,
                                             tr("Faild"),
                                             tr("Set cursor themes failed!"),
                                             KiranMessageBox::Ok);
                    emit sigSetCursorTheme(false, m_currentCursorTheme);
                }
            });
    return cursorWidget;
}

/**
 * @brief CursorThemes::convertToNomalImage 将xcursor转化出来的QImage转化为正常的图片
 * @param cursorImage xcursor转化出来的QImage
 * @return
 */
QImage CursorThemes::convertToNomalImage(const QImage &cursorImage)
{
    QRect rect(cursorImage.rect().bottomRight(), cursorImage.rect().topLeft());
    const quint32 *pixelData = reinterpret_cast<const quint32 *>(cursorImage.bits());
    for (int i = 0; i < cursorImage.height(); i++)
    {
        for (int j = 0; j < cursorImage.width(); j++)
        {
            if (*pixelData)
            {
                if (i < rect.left()) rect.setLeft(i);
                if (i > rect.right()) rect.setRight(i);
                if (j < rect.top()) rect.setTop(j);
                if (j > rect.bottom()) rect.setBottom(j);
                pixelData++;
            }
        }
    }
    return cursorImage.copy(rect.normalized());
}

QPixmap CursorThemes::loadCursorImage(const QString &name, const QString &theme, int size)
{
    std::string cursorName = name.toStdString();
    std::string cursorTheme = theme.toStdString();
    XcursorImage *xCursorImage = XcursorLibraryLoadImage(cursorName.c_str(), cursorTheme.c_str(), size);

    if (!xCursorImage)
    {
        KLOG_WARNING() << "Failed to load cursor image:" << name << "in theme" << theme;
        return QPixmap();
    }

    QImage img((uchar *)xCursorImage->pixels,
               xCursorImage->width, xCursorImage->height,
               QImage::Format_ARGB32_Premultiplied);

    QImage cursorImg = convertToNomalImage(img);
    XcursorImageDestroy(xCursorImage);
    return QPixmap::fromImage(cursorImg);
}
