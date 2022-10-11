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

#include "themes.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QButtonGroup>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMouseEvent>
#include <QPushButton>
#include "common/setting-brief-widget.h"
#include "cursor-themes/cursor-themes.h"
#include "dbus-interface/appearance-global-info.h"
#include "icon-themes/icon-themes.h"
#include "theme-widget-group.h"
#include "theme-widget.h"
#include "ui_themes.h"

#define DARK_THEME "Kiran-dark"
#define LIGHT_THEME "Kiran"

#define SETTING_THEME_NUM 2
#define SETTING_THEME_PATH "/usr/share/themes/"

using namespace std;
Themes::Themes(QWidget *parent) : QWidget(parent),
                                  ui(new Ui::Themes),
                                  m_iconThemes(nullptr),
                                  m_cursorThemes(nullptr)
{
    ui->setupUi(this);
    initUI();

    connect(AppearanceGlobalInfo::instance(), &AppearanceGlobalInfo::themeChanged, this, &Themes::handleThemeChange);
}

Themes::~Themes()
{
    delete ui;
    if (m_iconThemes != nullptr)
        delete m_iconThemes;
    if (m_cursorThemes != nullptr)
        delete m_cursorThemes;
}

void Themes::setPage(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

QSize Themes::sizeHint() const
{
    return QSize(670, 730);
}

/**
 * @brief Themes::initUI: 初始化主题设置界面
 * @return true: 通过dbus获取主题信息成功
 *         false: 通过dbus获取主题信息失败
 */
bool Themes::initUI()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->widget_effects->hide();

    if (!initThemesUI() || !initIconThemesUI() || !initCursorThemesUI())
        return false;

    return true;
}

/**
 * @brief Themes::initThemesUI:获取主题信息,并创建主题选择控件
 * @return true: 通过dbus获取主题信息成功
 *         false: 通过dbus获取主题信息失败
 */
bool Themes::initThemesUI()
{
    bool isGetThemes = getThemes(APPEARANCE_THEME_TYPE_GTK);
    if (!isGetThemes)
    {
        ui->widget_themes->hide();
        return false;
    }

    //get current  gtk theme
    if (!AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_GTK, m_currentTheme))
    {
        return false;
    }
    KLOG_INFO() << "Current theme is: " << m_currentTheme;

    /*TODO: 待主题包完成后，替换成可供用户设置的主题名,
     *后续根据用户可设置的主题名命名规则，确定是否要根据命名(如kiran*)来检索可设置的主题名*/
    m_themesName << DARK_THEME << LIGHT_THEME;

    createThemeWidget();
    return true;
}

/**
 * @brief Themes::initIconThemesUI: 获取图标主题信息，创建图标选择页面
 * @return true: 通过dbus获取主题信息成功
 *         false:通过dbus获取主题信息失败
 */
bool Themes::initIconThemesUI()
{
    //创建图标选择控件
    m_chooseIconWidget = new SettingBriefWidget(tr("Choose icon themes"));
    m_chooseIconWidget->setObjectName("chooseIconWidget");
    ui->verticalLayout_choose_widget->addWidget(m_chooseIconWidget);

    if (!AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_ICON, m_currIconThemes))
    {
        m_chooseIconWidget->setName(tr("Unknown"));
        return false;
    }

    m_chooseIconWidget->setName(m_currIconThemes);

    m_iconThemes = new IconThemes(ui->stackedWidget);
    m_iconThemes->installEventFilter(this);
    if (!m_iconThemes->initUI())
    {
        KiranMessageBox::message(nullptr, QObject::tr("Failed"),
                                 QObject::tr("Get icon themes failed!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    else
        ui->stackedWidget->addWidget(m_iconThemes);

    connect(m_chooseIconWidget, &SettingBriefWidget::clicked,
            [=] {
                ui->stackedWidget->setCurrentWidget(m_iconThemes);
            });

    connect(m_iconThemes, &IconThemes::sigSetIconTheme,
            [=](bool isSuccessful, QString newIconName) {
                m_chooseIconWidget->setName(newIconName);
                ui->stackedWidget->setCurrentIndex(0);
            });

    return true;
}

/**
 * @brief Themes::initCursorThemesUI:获取光标主题信息，创建图标选择页面
 * @return true: 通过dbus获取主题信息成功
 *         false:通过dbus获取主题信息失败
 */
bool Themes::initCursorThemesUI()
{
    //创建光标选择控件
    m_chooseCursorWidget = new SettingBriefWidget(tr("Choose cursor themes"));
    m_chooseCursorWidget->setObjectName("chooseCursorWidget");
    ui->verticalLayout_choose_widget->addWidget(m_chooseCursorWidget);

    if (!AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR, m_currCursorThemes))
    {
        m_chooseCursorWidget->setName(tr("Unknown"));
        return false;
    }
    m_chooseCursorWidget->setName(m_currCursorThemes);

    m_cursorThemes = new CursorThemes(ui->stackedWidget);
    m_cursorThemes->installEventFilter(this);
    if (!m_cursorThemes->initUI())
    {
        KiranMessageBox::message(nullptr, QObject::tr("Failed"),
                                 QObject::tr("Get cursor themes failed!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    else
        ui->stackedWidget->addWidget(m_cursorThemes);

    connect(m_chooseCursorWidget, &SettingBriefWidget::clicked,
            [=] {
                ui->stackedWidget->setCurrentWidget(m_cursorThemes);
            });

    connect(m_cursorThemes, &CursorThemes::sigSetCursorTheme,
            [=](bool isSuccessful, QString newThemeName) {
                m_chooseCursorWidget->setName(newThemeName);
                ui->stackedWidget->setCurrentIndex(0);
            });

    return true;
}

/**
 * @brief Themes::getThemes: 获取指定主题类型的主题信息，包括主题名及路径
 * @param themeType: 主题类型
 * @return true: 成功
 *         false:失败
 */
bool Themes::getThemes(int themeType)
{
    QString themesJson = nullptr;
    int themesNum;

    if (!AppearanceGlobalInfo::instance()->getAllThemes(themeType, themesJson))
    {
        return false;
    }
    themesNum = getJsonValueFromString(themesJson, &m_allThemesName, &m_allThemesPath);
    if (themesNum < 1)
    {
        KLOG_DEBUG() << "There is no theme to set";
        KiranMessageBox::message(nullptr, QObject::tr("Warning"),
                                 QObject::tr("There is no theme to set!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    return true;
}

/**
 * @brief Themes::getJsonValueFromString:将通过dbus传来的Json格式信息存储到QStringList中
 * @param jsonString:Json格式信息
 * @param themeName:所有主题名字
 * @param themePath:所有主题路径
 * @return:主题个数
 */
int Themes::getJsonValueFromString(QString jsonString, QStringList *themeName, QStringList *themePath)
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
        int themesNum = array.size();
        if (themesNum < 1)
            return -1;
        for (int i = 0; i < themesNum; i++)
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
                        themeName->insert(i, name);
                    }
                }
                if (themeInfoObj.contains("path"))
                {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if (themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        themePath->insert(i, path);
                    }
                }
            }
        }
    }
    return themeName->size();
}

/**
 * @brief Themes::createThemeWidget: 创建主题选择控件
 */
void Themes::createThemeWidget()
{
    m_themeWidgetGroup = new ThemeWidgetGroup(this);

    for (int i = 0; i < SETTING_THEME_NUM; i++)
    {
        ThemeWidget *themeWidget = new ThemeWidget(m_themesName.at(i), m_currentTheme, this);
        themeWidget->setAccessibleName(QString("ThemeWidget::%1").arg(m_themesName.at(i)));
        if (m_themesName.at(i) == m_currentTheme)
            m_themeWidgetGroup->setCurrentWidget(themeWidget);

        themeWidget->setTheme(m_themesName.at(i));
        //将依次主题信息控件添加进主题选择布局中，支持动态添加
        if (i % 2 == 0)
        {
            ui->gridLayout_themes->addWidget(themeWidget, i / 2, 0, Qt::AlignHCenter);
        }
        else
        {
            ui->gridLayout_themes->addWidget(themeWidget, i / 2, 1, Qt::AlignHCenter);
        }
        m_themeWidgetGroup->addWidget(themeWidget);

        connect(m_themeWidgetGroup, &ThemeWidgetGroup::themeWidgetChange,
                [=](ThemeWidget *preWidget, ThemeWidget *currWidget) {
                    Q_UNUSED(preWidget);
                    QString theme = currWidget->getTheme();
                    if (!QString::compare(m_currentTheme, theme))
                        return;
                    if (!AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_GTK, theme))
                        return;
                    if (!AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_METACITY, theme))
                        return;
                    KLOG_INFO() << "set themes successful:" << theme;
                });
    }
}

void Themes::handleThemeChange(int type, QString themeName)
{
    KLOG_INFO() << "handleThemeChange" << type << themeName;
    switch (type)
    {
    case APPEARANCE_THEME_TYPE_GTK:
    {
        QList<ThemeWidget *> widgets = m_themeWidgetGroup->getThemeWidgetList();
        foreach (ThemeWidget *widget, widgets)
        {
            if (widget->getTheme() == themeName)
            {
                widget->setSelectStatus(true, type);
                m_currentTheme = themeName;
            }
            else
                widget->setSelectStatus(false, type);
        }
        break;
    }
    case APPEARANCE_THEME_TYPE_CURSOR:
    {
        m_cursorThemes->updateCursorTheme(themeName);
        break;
    }
    case APPEARANCE_THEME_TYPE_ICON:
    {
        m_iconThemes->updateIconTheme(themeName);
        break;
    }
    default:
        break;
    }
}
