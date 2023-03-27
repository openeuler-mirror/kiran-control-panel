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

#include "theme-widget.h"
#include "kiran-frame/kiran-frame.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <style-palette.h>

using namespace Kiran;

#define DARK_THEME "Kiran-dark"
#define LIGHT_THEME "Kiran"
/**
 * @brief ThemeWidget::ThemeWidget :创建单个图标主题控件
 * @param iconSize:显示的图标大小
 * @param currentIconTheme:当前图标主题
 * @param iconThemeName:图标主题名字
 * @param iconStringList:要显示的图标名字列表
 * @param parent
 */
ThemeWidget::ThemeWidget(QSize iconSize, QString currentIconTheme,
                         QString iconThemeName, QStringList iconStringList, QWidget *parent) : QWidget(parent)
{
    // TODO:后续修改尺寸限制
    setMinimumWidth(455);

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setSpacing(4);
    mainVLayout->setMargin(0);

    m_iconImgWidget = new KiranFrame(this);
    m_iconImgWidget->setFixedHeight(70);
    m_iconImgWidget->setObjectName("iconImgWidget");
    m_iconImgWidget->setDrawBroder(false);
    m_iconImgWidget->setFixedBorderState(StylePalette::Checked);
    m_iconImgWidget->installEventFilter(this);

    QLabel *iconName = new QLabel(this);
    iconName->setText(iconThemeName);
    iconName->setAlignment(Qt::AlignHCenter);
    iconName->setObjectName("iconName");
    iconName->setStyleSheet("#iconName{color: #919191;"
                            "font-family: Noto Sans CJK SC regular;"
                            "font-size: 12px}");

    mainVLayout->addWidget(m_iconImgWidget, Qt::AlignLeft);
    mainVLayout->addWidget(iconName, Qt::AlignHCenter);

    this->setLayout(mainVLayout);

    QHBoxLayout *hLayout = new QHBoxLayout(m_iconImgWidget);
    hLayout->setSpacing(24);
    hLayout->setContentsMargins(QMargins(24, 0, 36, 0));

    for (QString icon : iconStringList)
    {
        QLabel *label = new QLabel(m_iconImgWidget);
        label->setFixedSize(iconSize);
        QPixmap pixmap(icon);
        label->setPixmap(pixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        hLayout->addWidget(label, Qt::AlignLeft);
    }

    m_iconSelectLabel = new QLabel(this);
    m_iconSelectLabel->setFixedSize(QSize(16, 16));

    // hLayout->addSpacerItem();
    hLayout->addWidget(m_iconSelectLabel, Qt::AlignRight);

    m_iconImgWidget->setLayout(hLayout);

    if (currentIconTheme == iconThemeName)
    {
        setSelectStatus(true, APPEARANCE_THEME_TYPE_ICON);
    }
}

/**
 * @brief ThemeWidget::ThemeWidget:创建单个主题控件
 * @param name: 主题名字
 * @param currentTheme:当前主题
 * @param parent
 */
ThemeWidget::ThemeWidget(QString name, QString currentTheme, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(10);

    QSizePolicy scSizePolicy = this->sizePolicy();
    scSizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    this->setSizePolicy(scSizePolicy);

    m_themeImgWidget = new KiranFrame(this);
    m_themeImgWidget->setFixedHeight(80);
    m_themeImgWidget->setFixedWidth(140);
    m_themeImgWidget->setDrawBroder(false);
    m_themeImgWidget->setFixedBorderState(StylePalette::Checked);
    m_themeImgWidget->installEventFilter(this);
    m_themeImgWidget->setBorderWidth(2);

    QLabel *themeName = new QLabel(this);
    themeName->setObjectName("themeName");
    themeName->setAlignment(Qt::AlignCenter);
    themeName->setAlignment(Qt::AlignHCenter);
    themeName->setStyleSheet("#themeName{color: #919191;"
                             "font-family: Noto Sans CJK SC regular;"
                             "font-size: 14px}");

    mainVLayout->addWidget(m_themeImgWidget, Qt::AlignLeft);
    mainVLayout->addWidget(themeName, Qt::AlignHCenter);
    this->setLayout(mainVLayout);

    QHBoxLayout *hLayout = new QHBoxLayout(m_themeImgWidget);
    hLayout->setMargin(0);
    hLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    hLayout->setSpacing(0);

    QLabel *img = new QLabel(m_themeImgWidget);
    img->setObjectName("img");
    img->setAlignment(Qt::AlignCenter);

    auto scaleAdjustSizeFunc = [](const QString &path) -> QPixmap
    {
        QPixmap pixmap(path);
        if (pixmap.isNull())
        {
            return pixmap;
        }
        return pixmap.scaled(QSize(136, 76), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };
    if (name == DARK_THEME)
    {
        img->setPixmap(scaleAdjustSizeFunc(":/kcp-appearance/images/theme-dark.png"));
        themeName->setText(tr("Dark Theme"));
    }
    else if (name == LIGHT_THEME)
    {
        img->setPixmap(scaleAdjustSizeFunc(":/kcp-appearance/images/theme-light.png"));
        themeName->setText(tr("Light Theme"));
    }
    else if (name == THEME_AUTO_NAME)
    {
        img->setPixmap(scaleAdjustSizeFunc(":/kcp-appearance/images/theme-auto.png"));
        themeName->setText(tr("Auto"));
    }

    // m_selectLabel = new QLabel(m_themeImgWidget);
    // m_selectLabel->setAlignment(Qt::AlignLeft);
    // m_selectLabel->setFixedSize(QSize(16, 16));

    hLayout->addWidget(img);
    // hLayout->addWidget(m_selectLabel);
    m_themeImgWidget->setLayout(hLayout);

    if (name == currentTheme)
    {
        setSelectStatus(true, APPEARANCE_THEME_TYPE_GTK);
    }
}

/**
 * @brief ThemeWidget::ThemeWidget 创建单个光标主题控件
 * @param cursorSize 显示的光标大小
 * @param currentCursorTheme 当前光标主题
 * @param cursorThemeName 光标主题名字
 * @param cursorListMap 要显示的光标图片
 * @param parent
 */
ThemeWidget::ThemeWidget(QSize cursorSize, QString currentCursorTheme,
                         QString cursorThemeName, const QList<QPixmap> &cursorListMap, QWidget *parent) : QWidget(parent)
{
    // TODO:后续修改尺寸限制
    setMinimumWidth(455);

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->setMargin(0);
    mainVLayout->setSpacing(4);

    QLabel *cursorName = new QLabel(this);
    cursorName->setText(cursorThemeName);
    cursorName->setAlignment(Qt::AlignHCenter);
    cursorName->setObjectName("cursorName");
    cursorName->setStyleSheet("#cursorName{color: #919191;"
                              "font-family: Noto Sans CJK SC regular;"
                              "font-size: 12px}");

    m_cursorWidget = new KiranFrame(this);
    m_cursorWidget->setObjectName("cursorWidget");
    m_cursorWidget->setFixedHeight(40);
    m_cursorWidget->setDrawBroder(false);
    m_cursorWidget->setFixedBorderState(StylePalette::Checked);
    m_cursorWidget->installEventFilter(this);

    mainVLayout->addWidget(m_cursorWidget);
    mainVLayout->addWidget(cursorName);
    //    setLayout(mainVLayout);

    QHBoxLayout *hLayout1 = new QHBoxLayout(m_cursorWidget);
    hLayout1->setSpacing(0);
    hLayout1->setMargin(0);
    hLayout1->setContentsMargins(24, 0, 36, 0);

    QWidget *cursorImgWidget = new QWidget(m_cursorWidget);
    cursorImgWidget->setObjectName("cursorImgWidget");
    QHBoxLayout *hLayout2 = new QHBoxLayout(cursorImgWidget);
    hLayout2->setSpacing(35);
    hLayout2->setMargin(0);
    for (QPixmap pixmap : cursorListMap)
    {
        QLabel *label = new QLabel(cursorImgWidget);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setFixedSize(cursorSize);
        label->setPixmap(pixmap);
        hLayout2->addWidget(label, Qt::AlignLeft);
    }
    hLayout2->addStretch();

    m_cursorSelectLabel = new QLabel(m_cursorWidget);
    m_cursorSelectLabel->setAlignment(Qt::AlignLeft);
    m_cursorSelectLabel->setFixedSize(QSize(16, 16));

    hLayout1->addWidget(cursorImgWidget, Qt::AlignLeft);
    hLayout1->addWidget(m_cursorSelectLabel);

    if (currentCursorTheme == cursorThemeName)
    {
        setSelectStatus(true, APPEARANCE_THEME_TYPE_CURSOR);
    }
}

ThemeWidget::~ThemeWidget()
{
}

/**
 * @brief ThemeWidget::setSelectStatus: 设置主题控件选中状态
 * @param selected 是否选中
 * @param themeType 主题类型
 */
void ThemeWidget::setSelectStatus(bool selected, int themeType)
{
    if (selected)
    {
        switch (themeType)
        {
        case APPEARANCE_THEME_TYPE_GTK:
            m_themeImgWidget->setDrawBroder(true);
            break;
        case APPEARANCE_THEME_TYPE_ICON:
            m_iconSelectLabel->setPixmap(QPixmap(":/kcp-appearance/images/indicator-selected.png"));
            m_iconImgWidget->setDrawBroder(true);
            break;
        case APPEARANCE_THEME_TYPE_CURSOR:
            m_cursorSelectLabel->setPixmap(QPixmap(":/kcp-appearance/images/indicator-selected.png"));
            m_cursorWidget->setDrawBroder(true);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (themeType)
        {
        case APPEARANCE_THEME_TYPE_GTK:
            m_themeImgWidget->setDrawBroder(false);
            break;
        case APPEARANCE_THEME_TYPE_ICON:
            m_iconSelectLabel->clear();
            m_iconImgWidget->setDrawBroder(false);
            break;
        case APPEARANCE_THEME_TYPE_CURSOR:
            m_cursorSelectLabel->clear();
            m_cursorWidget->setDrawBroder(false);
            break;
        default:
            break;
        }
    }
}

void ThemeWidget::setTheme(QString theme)
{
    m_theme = theme;
}

QString ThemeWidget::getTheme()
{
    return m_theme;
}

bool ThemeWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_iconImgWidget && event->type() == QEvent::MouseButtonPress)
    {
        emit clicked();
    }
    if (watched == m_themeImgWidget && event->type() == QEvent::MouseButtonPress)
    {
        emit clicked();
    }
    if (watched == m_cursorWidget && event->type() == QEvent::MouseButtonPress)
    {
        emit clicked();
    }
    return false;
}
