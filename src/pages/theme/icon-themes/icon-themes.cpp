#include "icon-themes.h"
#include "ui_icon-themes.h"
#include "dbus-interface/Appearance.h"
#include "dbus-interface/appearance-global-info.h"
#include "../theme-widget.h"
#include "../theme-widget-group.h"
#include <kiran-session-daemon/appearance-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>

#include <iostream>
#include <QVBoxLayout>
#include <QIcon>

static QStringList icons {"firefox.svg",
                          "file-manager.svg",
                          "smplayer.svg",
                          "preferences-system-notifications.svg",
                          "geeqie.svg",
                          "apparmor_view_profile.svg",
                          "awf.svg"};
/**
1、获取Kiran图标  themes：getAllThemes
2、搜索对应图标的路径下，找到指定的七个图标，保存至列表
3、创建图标类型ThemesWidget，并设置好对应的样式。将七个图标+选择状态图标 添加进该控件
4、给该ThemesWidget控件添加点击信号，若点击，切换选择状态图标，设置图标
5、主题设置主页面监听到设置图标信号，将当前设置页面设置为设置图标主页面
 */

IconThemes::IconThemes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IconThemes)
{
    ui->setupUi(this);
}

IconThemes::~IconThemes()
{
    delete ui;
}

bool IconThemes::initUI()
{
    m_currentIconTheme = ComKylinsecKiranSessionDaemonAppearanceInterface::instance()->icon_theme();

    if(!getIconThemes(APPEARANCE_THEME_TYPE_ICON))
    {
        return false;
    }

    createIconWidgets();
    return true;
}

bool IconThemes::getIconThemes(int themeType)
{
    QString iconThemesJson = nullptr;
    if(!AppearanceGlobalInfo::instance()->getAllThemes(themeType,iconThemesJson))
    {
        std::cout << "Get icon themes failed!" << std::endl;
        return false;
    }
    if(getJsonValueFromString(iconThemesJson)<=0)
    {
        std::cout << "Can't convert json string or there is no icon themes!" << std::endl;
        return false;
    }
    return true;
}

int IconThemes::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(),&jsonError);
    if( jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError ){
        std::cout << " please check the string "<< jsonString.toLocal8Bit().data();
        return -1;
    }
    if(jsonDocument.isArray())
    {
         QJsonArray array = jsonDocument.array();
         int iconThemesNum = array.size();
         if(iconThemesNum < 1 )
             return -1;
         for(int i=0 ;i<iconThemesNum ;i++)
         {
             QJsonValue value = array.at(i);
             if(value.type() == QJsonValue::Object)
             {
                 QJsonObject themeInfoObj = value.toObject();
                 if(themeInfoObj.contains("name"))
                 {
                    QJsonValue themeValue = themeInfoObj.value("name");
                    if(themeValue.isString())
                    {
                        QString name = themeValue.toVariant().toString();
                        m_iconThemes.insert(i,name);
                    }
                 }
                 if(themeInfoObj.contains("path"))
                 {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if(themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        m_iconThemesPath.insert(i,path);
                    }
                 }
             }
         }
    }
    return m_iconThemes.size();
}

void IconThemes::createIconWidgets()
{
    m_iconThemeWidgetGroup = new ThemeWidgetGroup(this);
    QStringList showIconsList;
    QVBoxLayout *vLayout = new QVBoxLayout(ui->widget_icon);
    vLayout->setMargin(0);
    vLayout->setSpacing(20);
    for (int i=0; i<m_iconThemes.size();i++)
    {
        if(m_iconThemes.at(i).startsWith("Kiran",Qt::CaseInsensitive))
        {

            QString path =  m_iconThemesPath.at(i) + "/apps/scalable/";
            QDir appsDir =  QDir(path);
            if(appsDir.exists())
            {
                for(int i=0; i< icons.size(); i++)
                {
                        showIconsList.append(path + icons.at(i));
                }
            }
            else
                continue;
            //new theme-widget
            ThemeWidget *themeWidget = new ThemeWidget(QSize(48,48),m_currentIconTheme,
                                                       m_iconThemes.at(i),showIconsList);
            vLayout->addWidget(themeWidget,Qt::AlignRight);

            if(m_iconThemes.at(i) == m_currentIconTheme)
                m_iconThemeWidgetGroup->setCurrentWidget(themeWidget);
            m_iconThemeWidgetGroup->addWidget(themeWidget);
            themeWidget->setTheme(m_iconThemes.at(i));
        }
    }
    ui->widget_icon->setLayout(vLayout);

    connect(m_iconThemeWidgetGroup,&ThemeWidgetGroup::themeWidgetChange,
            [=](ThemeWidget *preWidget,ThemeWidget *currWidget){
        if(preWidget)
        {
            if(preWidget ==  currWidget)
            {
                emit sigSetIconTheme(false);
                return ;
            }
            preWidget->setSelectStatus(false,APPEARANCE_THEME_TYPE_ICON);
        }

        currWidget->setSelectStatus(true,APPEARANCE_THEME_TYPE_ICON);
        if(AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_ICON,
                                                  currWidget->getTheme()))
        {
           std::cout << "set icon theme successful" << std::endl;
           emit sigSetIconTheme(true);
        }
        else
        {
           KiranMessageBox::message(nullptr,
                                    tr("Faild"),
                                    tr("Set icon themes failed!"),
                                    KiranMessageBox::Ok);
           emit sigSetIconTheme(false);
        }
    });
}
