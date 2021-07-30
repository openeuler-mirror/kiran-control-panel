#include "themes.h"
#include "ui_themes.h"
#include <QButtonGroup>
#include <QPushButton>
#include <iostream>
#include <QMouseEvent>
#include "dbus-interface/Appearance.h"
#include "dbus-interface/appearance-global-info.h"
#include "icon-themes/icon-themes.h"
#include "cursor-themes/cursor-themes.h"
#include "common/chooser-widget.h"
#include "theme-widget.h"
#include "theme-widget-group.h"
#include <kiran-session-daemon/appearance-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>

#define DARK_THEME      "KiranM-dark"
#define LIGHT_THEME     "KiranM"

#define SETTING_THEME_NUM       2
#define SETTING_THEME_PATH      "/usr/share/themes/"

using namespace std;
Themes::Themes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Themes),
    m_iconThemes(nullptr),
    m_cursorThemes(nullptr)
{
    ui->setupUi(this);
    initUI();
}

Themes::~Themes()
{
    delete ui;
    if(m_iconThemes != nullptr)
        delete m_iconThemes;
}

void Themes::setPage(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

// 一、深浅色主题设计思路
//1、获取深浅色主题名字 、个数 ，并根据个数创建主题button，放入QButtonGroup中
//2、根据主题名字创建对应的主题选择按钮,加入到QButtonGroup中(传入主题名、对应主题图片名)
//3、监听QButtonGroup鼠标点击信号，判断当前点击的主题按钮是哪一个， 并通过DBus进行主题设置

bool Themes::initUI()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->widget_effects->hide();

    initThemesUI();
    initIconThemesUI();
    initCursorThemesUI();

    return true;
}

bool Themes::initThemesUI()
{
    bool isGetThemes = getThemes(APPEARANCE_THEME_TYPE_GTK);
    if(!isGetThemes)
    {
        ui->widget_themes->hide();
        return false;
    }

    //get current  gtk theme
    m_currentTheme = AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_GTK);
    cout << "Current theme is: " << m_currentTheme.toStdString() << endl;

    /*TODO: 待主题包完成后，替换成可供用户设置的主题名,
     *后续根据用户可设置的主题名命名规则，确定是否要根据命名(如kiran*)来检索可设置的主题名*/
    m_themesName << DARK_THEME << LIGHT_THEME;

    createThemeWidget();
    return true;
}

void Themes::initIconThemesUI()
{
    //创建图标选择控件
    m_chooseIconWidget = new ChooserWidget(tr("Choose icon theme"));
    m_chooseIconWidget->setObjectName("chooseIconWidget");
    ui->verticalLayout_choose_widget->addWidget(m_chooseIconWidget);

    m_currIconThemes = AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_ICON);
    m_chooseIconWidget->setName(m_currIconThemes);

    connect(m_chooseIconWidget,&ChooserWidget::clicked,
            [=]{
        if(m_iconThemes == nullptr)
        {
            m_iconThemes = new IconThemes(ui->stackedWidget);
            m_iconThemes->installEventFilter(this);
            if(!m_iconThemes->initUI())
            {
                KiranMessageBox::message(nullptr,QObject::tr("Failed"),
                                         QObject::tr("Get icon themes failed!"),
                                         KiranMessageBox::Ok);
                m_chooseIconWidget->setName(tr("Unknown"));
                return ;
            }
            else
               ui->stackedWidget->addWidget(m_iconThemes);
        }
        ui->stackedWidget->setCurrentWidget(m_iconThemes);
        connect(m_iconThemes,&IconThemes::sigSetIconTheme,
                [=](bool isSuccessful){
           if(isSuccessful)
           {
                m_chooseIconWidget->setName(AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_ICON));
           }
           ui->stackedWidget->setCurrentIndex(0);
        });
    });
}

void Themes::initCursorThemesUI()
{
    //创建光标选择控件
    m_chooseCursorWidget = new ChooserWidget(tr("Choose cursor widget"));
    m_chooseCursorWidget->setObjectName("chooseCursorWidget");

    m_currCursorThemes = AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR);
    m_chooseCursorWidget->setName(m_currCursorThemes);
    ui->verticalLayout_choose_widget->addWidget(m_chooseCursorWidget);

    connect(m_chooseCursorWidget,&ChooserWidget::clicked,
            [=]{
        if(m_cursorThemes == nullptr)
        {
            m_cursorThemes = new CursorThemes(ui->stackedWidget);
            m_cursorThemes->installEventFilter(this);
            if(!m_cursorThemes->initUI())
            {
                KiranMessageBox::message(nullptr,QObject::tr("Failed"),
                                         QObject::tr("Get cursor themes failed!"),
                                         KiranMessageBox::Ok);
                m_chooseCursorWidget->setName(tr("Unknown"));
                return;
            }
            else
            {
                ui->stackedWidget->addWidget(m_cursorThemes);
            }
        }
        ui->stackedWidget->setCurrentWidget(m_cursorThemes);
        connect(m_cursorThemes,&CursorThemes::sigSetCursorTheme,
                [=](bool isSuccessful){
           if(isSuccessful)
           {
                m_chooseCursorWidget->setName(AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR));
           }
           ui->stackedWidget->setCurrentIndex(0);
        });
    });


}

bool Themes::getThemes(int themeType)
{
    QString themesJson=nullptr;
    int themesNum;

    if(!AppearanceGlobalInfo::instance()->getAllThemes(themeType , themesJson))
    {
        return false;
    }
    themesNum= getJsonValueFromString(themesJson,&m_allThemesName,&m_allThemesPath);
    if(themesNum<1)
    {
        cout << "There is no theme to set" << endl;
        KiranMessageBox::message(nullptr,QObject::tr("Warning"),
                                 QObject::tr("There is no theme to set!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    else
    {
        cout << "Theme number is:" << themesNum << endl;
        cout << "themes name = ";
        foreach (QString name, m_allThemesName) {
            cout << name.toStdString() << ","  ;
        }
        cout << endl;
        cout << "theme path = ";
        foreach (QString path, m_allThemesPath) {
            cout << path.toStdString() << ",";
        }
        cout << endl;
    }
    return true;
}


int Themes::getJsonValueFromString(QString jsonString, QStringList *themeName, QStringList *themePath)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(),&jsonError);
    if( jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError ){
        cout << " please check the string "<< jsonString.toLocal8Bit().data();
        return -1;
    }
    if(jsonDocument.isArray())
    {
         QJsonArray array = jsonDocument.array();
         int themesNum = array.size();
         if(themesNum < 1 )
             return -1;
         for(int i=0 ;i<themesNum ;i++)
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
                        themeName->insert(i,name);
                    }
                 }
                 if(themeInfoObj.contains("path"))
                 {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if(themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        themePath->insert(i,path);
                    }
                 }
             }
         }
    }
    return themeName->size();
}

void Themes::createThemeWidget()
{   
    m_themeWidgetGroup = new ThemeWidgetGroup(this);

    for(int i=0 ;i<SETTING_THEME_NUM; i++)
    {
        ThemeWidget *themeWidget = new ThemeWidget(m_themesName.at(i),m_currentTheme,this);
        if(m_themesName.at(i) == m_currentTheme)
            m_themeWidgetGroup->setCurrentWidget(themeWidget);

        themeWidget->setTheme(m_themesName.at(i));
        //将依次主题信息控件添加进主题选择布局中，支持动态添加
        if(i%2==0)
        {
            ui->gridLayout_themes->addWidget(themeWidget,i/2,0,Qt::AlignHCenter);
        }
        else
        {
            ui->gridLayout_themes->addWidget(themeWidget,i/2,1,Qt::AlignHCenter);
        }
        m_themeWidgetGroup->addWidget(themeWidget);

        connect(m_themeWidgetGroup,&ThemeWidgetGroup::themeWidgetChange,
                [=](ThemeWidget *preWidget, ThemeWidget* currWidget){
            if(preWidget)
            {
                preWidget->setSelectStatus(false,APPEARANCE_THEME_TYPE_GTK);
            }
            currWidget->setSelectStatus(true,APPEARANCE_THEME_TYPE_GTK);

            QString theme = currWidget->getTheme();
            if(!QString::compare(m_currentTheme,theme))
                return;
            if(!AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_GTK, theme))
            {
                cout << "setTheme failed!"
                     << " theme type is: " << APPEARANCE_THEME_TYPE_GTK
                     << " theme name is: " << theme.toStdString()
                     << endl;
                return ;
            }
            cout << "set themes successful:" << theme.toStdString() <<endl;
            m_currentTheme = theme;
        });
    }
}

