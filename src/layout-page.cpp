#include "layout-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <widget-property-helper.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QScrollBar>
#include "dbus-wrapper/KSKKeyboardProxy.h"
#include "dbus-wrapper/dbus-wrapper.h"
#include "ui_layout-page.h"
#include "widgets/choose-item.h"
#include "widgets/layout-list.h"
LayoutPage::LayoutPage(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::LayoutPage)
{
    ui->setupUi(this);

    DbusWrapper *dbusWrapper = new DbusWrapper;
    m_keyboardInterface = dbusWrapper->getKeyboardInterface();
    dbusWrapper->deleteLater();
    dbusWrapper = nullptr;

    initUI();
    getValidLayout();
    createLayoutItem();
}

LayoutPage::~LayoutPage()
{
    delete ui;
}

void LayoutPage::initUI()
{
    m_vLayout = new QVBoxLayout();
    m_vLayout->setMargin(0);
    m_vLayout->setContentsMargins(0, 0, 0, 0);
    m_vLayout->setSpacing(10);
    ui->scroll_layout_selector->setLayout(m_vLayout);
    ui->stackedWidget->setCurrentWidget(ui->page_layoutList);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_page_add, Kiran::BUTTON_Default);

    connect(ui->btn_edit, &QPushButton::clicked, this, &LayoutPage::setEditMode);
    connect(ui->btn_add, &QToolButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_layoutAddition);
            });
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
        if (0 < getJsonValueFromString(json))
        {
            QStringList validLayoutCountry;
            QMap<QString, QString>::const_iterator i = m_layoutMap.begin();
            while (i != m_layoutMap.end())
            {
                validLayoutCountry.append(i.value());
                ++i;
            }
            ui->widget_layout_list->setCountryList(validLayoutCountry);
        }
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
                        //                        KLOG_INFO() << countryName;
                    }
                }
                if (obj.contains("layout_name"))
                {
                    QJsonValue value = obj.value("layout_name");
                    if (value.isString())
                    {
                        layoutName = value.toVariant().toString();
                        //                        KLOG_INFO() << layoutName;
                    }
                }
                m_layoutMap.insert(layoutName, countryName);
            }
        }
    }
    return m_layoutMap.size();
}

void LayoutPage::createLayoutItem()
{
    //addLayout("vn");
    //addLayout("tr alt");
    m_layoutList = m_keyboardInterface->layouts();
    m_layout = m_layoutList.first();
    for (int i = 0; i < m_layoutList.size(); i++)
    {
        QString layoutName = m_layoutList.at(i);
        QString countryName = m_layoutMap.value(layoutName);
        KLOG_INFO() << countryName;
        ChooseItem *chooseItem = new ChooseItem(this);
        chooseItem->setNames(countryName, layoutName);
        m_vLayout->addWidget(chooseItem);
        m_itemList.append(chooseItem);

        if (i == 0)
        {
            chooseItem->setSelected(true);
        }

        connect(chooseItem, &ChooseItem::clicked,
                [=] {
                    QString selectedLayoutName = chooseItem->getLayoutName();
                    if (m_layout != selectedLayoutName)
                    {
                        QDBusPendingReply<> reply = m_keyboardInterface->ApplyLayout(selectedLayoutName);
                        reply.waitForFinished();
                        if (reply.isError() || !reply.isValid())
                        {
                            KLOG_DEBUG() << "Call ApplyLayout method failed "
                                         << " Error: " << reply.error().message();
                            return;
                        }
                        else
                        {
                            m_layoutList.clear();
                            ///TODO:是否需要自己更新layoutList而不是通过dbus获取
                            m_layoutList = m_keyboardInterface->layouts();
                            m_layout = m_layoutList.first();
                            //emit layoutSelectChanged(selectedLayoutName);
                            updateLayout(m_layoutList);
                        }
                    }
                });
        connect(this, &LayoutPage::layoutSelectChanged, chooseItem, &ChooseItem::seletedLayoutChanged);

        connect(chooseItem, &ChooseItem::sigDelete, this, &LayoutPage::deleteLayout);
    }
}

/* 属性变化后更新布局*/
void LayoutPage::updateLayout(QStringList layoutList)
{
    if (!layoutList.isEmpty())
    {
        QString countryName;
        if (layoutList.size() > m_itemList.size())  // add
        {
            for (int i = 0; i < (m_layoutList.size() - m_itemList.size()); i++)
            {
                ChooseItem *item = new ChooseItem(this);
                m_vLayout->addWidget(item);
                m_itemList.append(item);
            }
        }
        else if (layoutList.size() < m_itemList.size())  //delete
        {
            for (int i = 0; i < (m_itemList.size() - layoutList.size()); i++)
            {
                m_itemList.removeLast();
            }
        }

        for (int i = 0; i < layoutList.size(); i++)
        {
            countryName = m_layoutMap.value(layoutList.at(i));
            m_itemList.at(i)->setNames(countryName, layoutList.at(i));
        }
        m_itemList.first()->setSelected(true);
    }
    else
    {
        foreach (ChooseItem *item, m_itemList)
        {
            m_itemList.removeOne(item);
            delete item;
            item = nullptr;
        }
        m_itemList.clear();
        ui->widget_layout_list->hide();
    }
}

void LayoutPage::addLayout(QString layoutName)
{
    QDBusPendingReply<> reply = m_keyboardInterface->AddLayout(layoutName);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call AddLayout method failed "
                     << " Error: " << reply.error().message();
        KiranMessageBox::message(nullptr, tr("Add Layout"),
                                 QString(tr("Add Layout failed: %1").arg(reply.error().message())),
                                 KiranMessageBox::Ok);
        return;
    }
    else
    {
        ChooseItem *item = new ChooseItem(this);
        item->setNames(m_layoutMap.value(layoutName), layoutName);
        m_vLayout->addWidget(item);
        m_itemList.append(item);
        m_layoutList.append(layoutName);
    }
}

void LayoutPage::deleteLayout(QString deletedLayout)
{
    QDBusPendingReply<> reply = m_keyboardInterface->DelLayout(deletedLayout);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call DelLayout method failed "
                     << " Error: " << reply.error().message();
        KiranMessageBox::message(nullptr, tr("Delete Layout"),
                                 QString(tr("Delete Layout failed: %1").arg(reply.error().message())),
                                 KiranMessageBox::Ok);
        return;
    }
    else
    {
        ChooseItem *item = dynamic_cast<ChooseItem *>(sender());
        ///TODO:是否需要自己更新layoutList而不是通过dbus获取
        m_layoutList = m_keyboardInterface->layouts();
        //界面上删除选择项
        m_itemList.removeOne(item);
        delete item;
        item = nullptr;
        updateLayout(m_layoutList);
    }
}

void LayoutPage::setEditMode()
{
    if (!m_editFlag)
    {
        foreach (ChooseItem *item, m_itemList)
        {
            item->setEditMode(true);
            m_editFlag = true;
        }
        ui->btn_edit->setText(tr("Finish"));
    }
    else
    {
        foreach (ChooseItem *item, m_itemList)
        {
            item->setEditMode(false);
            m_editFlag = false;
        }
        ui->btn_edit->setText(tr("Edit"));
    }
}
