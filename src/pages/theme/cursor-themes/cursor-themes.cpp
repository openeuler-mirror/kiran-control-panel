/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

// clang-format off
#include <kiran-message-box.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include "../theme-widget-group.h"
#include "../theme-widget.h"
#include "cursor-themes.h"
#include "dbus-interface/Appearance.h"
#include "dbus-interface/appearance-global-info.h"
#include <X11/Xcursor/Xcursor.h>
#include <kiran-log/qt5-log-i.h>
// clang-format on

static QStringList cursors{"arrow", "size_bdiag", "size_fdiag", "size_ver", "size_hor", "pointer"};

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
        KLOG_DEBUG() << "get cursor themes failed";
        return false;
    }
    if (getJsonValueFromString(themeJson) <= 0)
    {
        KLOG_DEBUG() << "Can't convert json string or there is no cursor themes!";
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
        KLOG_DEBUG() << " please check the string " << jsonString.toLocal8Bit().data();
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
    ThemeWidgetGroup *themeWidgetGroup = new ThemeWidgetGroup(this);

    QWidget *cursorWidget = new QWidget(this);
    QVBoxLayout *mainVLayout = new QVBoxLayout(cursorWidget);
    mainVLayout->setSpacing(4);
    mainVLayout->setMargin(0);
    int size = 18;

    for (int i = 0; i < m_cursorThemesName.size(); i++)
    {
        //        if (m_cursorThemesName.at(i).startsWith("Kiran", Qt::CaseInsensitive))
        //        {
        QList<QPixmap> cursorMap;
        QDir cursorThemesDir(m_cursorThemesPath.at(i));

        for (int j = 0; j < cursors.size(); j++)
        {
            QString cursor = cursors.at(j);
            //将xcursor转化为image
            std::string cursorName = cursor.toStdString();
            std::string cursorTheme = cursorThemesDir.dirName().toStdString();
            XcursorImage *xCursorImage = XcursorLibraryLoadImage(cursorName.c_str(), cursorTheme.c_str(), size);

            QImage cursorImg((uchar *)xCursorImage->pixels,
                             xCursorImage->width, xCursorImage->height,
                             QImage::Format_ARGB32_Premultiplied);

            XcursorImageDestroy(xCursorImage);

            cursorImg = convertToNomalImage(cursorImg);

            cursorMap.append(QPixmap::fromImage(cursorImg));
        }

        ThemeWidget *themeWidget = new ThemeWidget(QSize(18, 18), m_currentCursorTheme,
                                                   m_cursorThemesName.at(i), cursorMap);
        mainVLayout->addWidget(themeWidget);
        themeWidgetGroup->addWidget(themeWidget);

        if (m_currentCursorTheme == m_cursorThemesName.at(i))
        {
            themeWidgetGroup->setCurrentWidget(themeWidget);
        }
        themeWidget->setTheme(m_cursorThemesName.at(i));
        //}
    }
    mainVLayout->addStretch();

    connect(themeWidgetGroup, &ThemeWidgetGroup::themeWidgetChange,
            [=](ThemeWidget *preWidget, ThemeWidget *currWidget) {
                if (preWidget)
                {
                    if (preWidget == currWidget)
                    {
                        emit sigSetCursorTheme(false);
                        return;
                    }
                    preWidget->setSelectStatus(false, APPEARANCE_THEME_TYPE_CURSOR);
                }

                currWidget->setSelectStatus(true, APPEARANCE_THEME_TYPE_CURSOR);
                if (AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_CURSOR,
                                                               currWidget->getTheme()))
                {
                    KLOG_INFO() << "set cursor theme successful";
                    m_currentCursorTheme = currWidget->getTheme();
                    emit sigSetCursorTheme(true);
                }
                else
                {
                    KiranMessageBox::message(nullptr,
                                             tr("Faild"),
                                             tr("Set cursor themes failed!"),
                                             KiranMessageBox::Ok);
                    emit sigSetCursorTheme(false);
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
    const quint32 *pixelData = reinterpret_cast<const quint32 *>(cursorImage.scanLine(0));
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
