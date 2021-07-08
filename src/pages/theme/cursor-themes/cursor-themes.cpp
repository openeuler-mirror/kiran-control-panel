#include "cursor-themes.h"
#include "dbus-interface/appearance-global-info.h"
#include "dbus-interface/Appearance.h"
#include "../theme-widget-group.h"
#include "../theme-widget.h"
#include "cursor/xcursortheme.h"
#include <QVBoxLayout>
#include <QLabel>
#include <iostream>
#include <QIcon>
#include <kiran-session-daemon/appearance-i.h>
#include <kiran-message-box.h>

static QStringList cursors {"arrow","size_bdiag","size_fdiag","zoom-in","wait","pointer"};

CursorThemes::CursorThemes(QWidget *parent):
    QWidget(parent)
{
}

bool CursorThemes::initUI()
{
    if(!getCursorThemes(APPEARANCE_THEME_TYPE_CURSOR))
    {
        return false;
    }

    m_currentCursorTheme = AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR);
    QVBoxLayout * mainVLayout = new QVBoxLayout(this);
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

bool CursorThemes::getCursorThemes(int Type)
{
    QString themeJson = nullptr;
    if(!AppearanceGlobalInfo::instance()->getAllThemes(Type,themeJson))
    {
        std::cout << "get cursor themes failed" << std::endl;
        return false;
    }
    if(getJsonValueFromString(themeJson)<=0)
    {
        std::cout << "Can't convert json string or there is no cursor themes!" << std::endl;
        return false;
    }
    return true;
}

int CursorThemes::getJsonValueFromString(QString jsonString)
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
                        m_cursorThemesName.insert(i,name);
                    }
                 }
                 if(themeInfoObj.contains("path"))
                 {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if(themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        m_cursorThemesPath.insert(i,path);
                    }
                 }
             }
         }
    }
    return m_cursorThemesName.size();
}

QWidget* CursorThemes::createCursorWidget()
{
    ThemeWidgetGroup *themeWidgetGroup = new ThemeWidgetGroup(this);

    QWidget *cursorWidget = new QWidget(this);
    QVBoxLayout *mainVLayout = new QVBoxLayout(cursorWidget);
    mainVLayout->setSpacing(4);
    mainVLayout->setMargin(0);
    //cursorWidget->setLayout(mainVLayout);

    for(int i= 0; i<m_cursorThemesName.size(); i++)
    {
        if(m_cursorThemesName.at(i).startsWith("Kiran",Qt::CaseInsensitive))
        {
            QList<QPixmap> cursorMap;
            QString cursorDir =  m_cursorThemesPath.at(i) + "/cursors/";
            XCursorTheme *cursorTheme = new XCursorTheme(QDir(cursorDir));

            for(int j=0; j< cursors.size(); j++)
            {
                QString cursor = cursors.at(j);
                QImage image = cursorTheme->loadImage(cursor,16);
                cursorMap.append(QPixmap::fromImage(image));
            }

            ThemeWidget *themeWidget = new ThemeWidget(QSize(16,16), m_currentCursorTheme,
                                                       m_cursorThemesName.at(i),cursorMap);
            mainVLayout->addWidget(themeWidget);
            themeWidgetGroup->addWidget(themeWidget);

            if(m_currentCursorTheme == m_cursorThemesName.at(i))
            {
                themeWidgetGroup->setCurrentWidget(themeWidget);
            }
            themeWidget->setTheme(m_cursorThemesName.at(i));
        }
    }
    mainVLayout->addStretch();

    connect(themeWidgetGroup,&ThemeWidgetGroup::themeWidgetChange,
            [=](ThemeWidget *preWidget,ThemeWidget *currWidget){
        if(preWidget)
        {
            if(preWidget == currWidget)
            {
                emit sigSetCursorTheme(false);
                return ;
            }
            preWidget->setSelectStatus(false,APPEARANCE_THEME_TYPE_CURSOR);
        }

        currWidget->setSelectStatus(true,APPEARANCE_THEME_TYPE_CURSOR);
        if(AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_CURSOR,
                                                  currWidget->getTheme()))
        {
           std::cout << "set cursor theme successful" << std::endl;
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
