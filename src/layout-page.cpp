#include "layout-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <widget-property-helper.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QScrollBar>
#include "dbus-wrapper/KeyboardBackEndProxy.h"
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
    connect(m_keyboardInterface.data(), &KeyboardBackEndProxy::layoutsChanged,
            [this](QStringList layoutList) {
                KLOG_INFO() << "get layoutsChanged signal: " << layoutList;
                m_layoutList = layoutList;
                //更新界面
                updateLayout();
            });

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
    ui->layout_selector->setLayout(m_vLayout);
    ui->stackedWidget->setCurrentWidget(ui->page_layoutList);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_page_add, Kiran::BUTTON_Default);
    ui->btn_page_add->setDisabled(true);

    connect(ui->btn_edit, &QPushButton::clicked, this, &LayoutPage::setEditMode);
    connect(ui->btn_add, &QToolButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_layoutAddition);
            });
    connect(ui->btn_page_add, &QPushButton::clicked,
            [this] {
                QString additionLayout;
                QString countryName = ui->widget_layout_list->getSelectedCountry();
                QMap<QString, QString>::const_iterator i = m_layoutMap.begin();
                while (i != m_layoutMap.end())
                {
                    if (i.value() == countryName)
                    {
                        additionLayout = i.key();
                        break;
                    }
                    ++i;
                }
                if (!m_layoutList.contains(additionLayout))
                {
                    if (addLayout(additionLayout))
                        ui->stackedWidget->setCurrentWidget(ui->page_layoutList);
                }
                else
                    KiranMessageBox::message(nullptr,
                                             tr("Failed"),
                                             tr("You have added this keyboard layout!"),
                                             KiranMessageBox::Ok);
            });
    connect(ui->widget_layout_list, &LayoutList::itemChanged,
            [this](QString countryName) {
                ui->btn_page_add->setDisabled(false);
            });
    connect(ui->btn_return, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_layoutList);
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
        if (!m_layoutMap.contains(layoutName))
        {
            continue;
        }
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

        connect(chooseItem, &ChooseItem::clicked, this, &LayoutPage::chooseItemClicked);
        connect(chooseItem, &ChooseItem::sigDelete, this, &LayoutPage::deleteLayout);
    }
}

/* 属性变化后更新布局*/
void LayoutPage::updateLayout()
{
    if (!m_layoutList.isEmpty())
    {
        QString countryName;
        if (m_layoutList.size() > m_itemList.size())  // add
        {
            KLOG_INFO() << "add";
            for (int i = 0; i < (m_layoutList.size() - m_itemList.size()); i++)
            {
                //添加缺少的选择项
                ChooseItem *item = new ChooseItem(this);
                connect(item, &ChooseItem::clicked, this, &LayoutPage::chooseItemClicked);
                connect(item, &ChooseItem::sigDelete, this, &LayoutPage::deleteLayout);

                m_vLayout->addWidget(item);
                m_itemList.append(item);
            }
        }
        else if (m_layoutList.size() < m_itemList.size())  //delete
        {
            KLOG_INFO() << "delete";
            for (int i = 0; i < (m_itemList.size() - m_layoutList.size()); i++)
            {
                //删除多余的选择项
                ChooseItem *item = m_itemList.takeLast();
                delete item;
                item = nullptr;
            }
        }

        for (int i = 0; i < m_layoutList.size(); i++)
        {
            countryName = m_layoutMap.value(m_layoutList.at(i));
            m_itemList.at(i)->setNames(countryName, m_layoutList.at(i));
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

bool LayoutPage::addLayout(QString layoutName)
{
    if (m_layoutMap.contains(layoutName))
    {
        QDBusPendingReply<> reply = m_keyboardInterface->AddLayout(layoutName);
        reply.waitForFinished();
        if (reply.isError() || !reply.isValid())
        {
            KLOG_DEBUG() << "Call AddLayout method failed "
                         << " Error: " << reply.error().message();
            KiranMessageBox::message(nullptr, tr("Add Layout"),
                                     reply.error().message(),
                                     KiranMessageBox::Ok);
            return false;
        }
        else
        {
            ChooseItem *item = new ChooseItem(this);
            item->setNames(m_layoutMap.value(layoutName), layoutName);
            connect(item, &ChooseItem::clicked, this, &LayoutPage::chooseItemClicked);
            //connect(this, &LayoutPage::layoutSelectChanged, chooseItem, &ChooseItem::seletedLayoutChanged);

            connect(item, &ChooseItem::sigDelete, this, &LayoutPage::deleteLayout);

            m_vLayout->addWidget(item);
            m_itemList.append(item);
            m_layoutList.append(layoutName);
            if (m_editFlag)
                item->setEditMode(true);
        }
    }
    else
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 QString(tr("The %1 keyboard layout does not exist!")).arg(layoutName),
                                 KiranMessageBox::Ok);
        return false;
    }
    return true;
}

void LayoutPage::chooseItemClicked()
{
    ChooseItem *item = dynamic_cast<ChooseItem *>(sender());

    QString selectedLayoutName = item->getLayoutName();
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
            m_layout = selectedLayoutName;
            ///TODO:是否需要自己更新layoutList而不是通过dbus获取
            m_layoutList = m_keyboardInterface->layouts();
            //emit layoutSelectChanged(selectedLayoutName);
            updateLayout();
        }
    }
}

void LayoutPage::deleteLayout(QString deletedLayout)
{
    if (m_layout == deletedLayout)
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 tr("The keyboard layout is currently in use and cannot be deleted!"),
                                 KiranMessageBox::Ok);
        return;
    }
    if (m_layoutList.contains(deletedLayout))
    {
        QDBusPendingReply<> reply = m_keyboardInterface->DelLayout(deletedLayout);
        reply.waitForFinished();
        if (reply.isError() || !reply.isValid())
        {
            KLOG_DEBUG() << "Call DelLayout method failed "
                         << " Error: " << reply.error().message();
            KiranMessageBox::message(nullptr, tr("Delete Layout"),
                                     reply.error().message(),
                                     KiranMessageBox::Ok);
            return;
        }
        else
        {
            m_layoutList.clear();
            ChooseItem *item = dynamic_cast<ChooseItem *>(sender());
            ///TODO:是否需要自己更新layoutList而不是通过dbus获取
            m_layoutList = m_keyboardInterface->layouts();
            //界面上删除选择项
            m_itemList.removeOne(item);
            delete item;
            item = nullptr;
        }
    }
    else
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 QString(tr("You do not appear to have added %1 keyboard layout!")).arg(deletedLayout),
                                 KiranMessageBox::Ok);
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
