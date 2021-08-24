#include "layout-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QScrollBar>
#include "dbus-wrapper/KSKKeyboardProxy.h"
#include "dbus-wrapper/dbus-wrapper.h"
#include "ui_layout-page.h"
#include "widgets/choose-item.h"
LayoutPage::LayoutPage(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::LayoutPage)
{
    ui->setupUi(this);

    DbusWrapper *dbusWrapper = new DbusWrapper;
    m_keyboardInterface = dbusWrapper->getKeyboardInterface();
    dbusWrapper->deleteLater();
    dbusWrapper = nullptr;

    initUI();
    createLayoutItem();
    //getValidLayout();
}

LayoutPage::~LayoutPage()
{
    delete ui;
}

void LayoutPage::initUI()
{
    QWidget *layoutsWidget = new QWidget(this);
    ui->hLayout_keyLayout->addWidget(layoutsWidget);
    QScrollBar *scrollBar = new QScrollBar(Qt::Vertical, this);
    ui->hLayout_keyLayout->addWidget(scrollBar);
}

void LayoutPage::getValidLayout()
{
    QDBusPendingReply<QString> reply = m_keyboardInterface->GetValidLayouts();
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call GetValidLayouts method failed "
                     << " Error: " << reply.error().message();
        return;
    }
    else
    {
        QString json = reply.argumentAt(0).toString();
        getJsonValueFromString(json);
    }
}

int LayoutPage::getJsonValueFromString(QString jsonString)
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
        int layoutNum = array.size();
        if (layoutNum < 1)
            return -1;
        for (int i = 0; i < layoutNum; i++)
        {
            QString countryName;
            QString layoutName;
            QJsonValue value = array.at(i);
            if (value.type() == QJsonValue::Object)
            {
                QJsonObject obj = value.toObject();
                if (obj.contains("country_name"))
                {
                    QJsonValue value = obj.value("country_name");
                    if (value.isString())
                    {
                        countryName = value.toVariant().toString();
                        KLOG_INFO() << countryName;
                    }
                }
                if (obj.contains("layout_name"))
                {
                    QJsonValue value = obj.value("layout_name");
                    if (value.isString())
                    {
                        layoutName = value.toVariant().toString();
                        KLOG_INFO() << layoutName;
                    }
                }
                m_layoutMap.insert(countryName, layoutName);
            }
        }
    }
    return m_layoutMap.size();
}

void LayoutPage::createLayoutItem()
{
    m_layoutList = m_keyboardInterface->layouts();
    foreach (QString layoutName, m_layoutList)
    {
    }
}
