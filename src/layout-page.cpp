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

    connect(ui->btn_edit, &QPushButton::clicked, this, &LayoutPage::setEditMode);
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
            m_firstItem = chooseItem;
            m_firstItem->setSelected(true);
        }

        connect(chooseItem, &ChooseItem::clicked,
                [=] {
                    QString selectedLayoutName = chooseItem->getLayoutName();
                    if (m_firstItem->getLayoutName() != selectedLayoutName)
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
                            m_layoutList = m_keyboardInterface->layouts();
                        }
                        updateLayout(m_layoutList);
                    }
                });
        connect(this, &LayoutPage::layoutSelectChanged, chooseItem, &ChooseItem::seletedLayoutChanged);

        connect(chooseItem, &ChooseItem::sigDelete, this, &LayoutPage::deleteLayout);
    }
}

void LayoutPage::updateLayout(QStringList layoutList)
{
    if (!layoutList.isEmpty())
    {
        QString countryName;
        if (layoutList.size() > m_itemList.size())  // add
        {
            for (i = 0; i < (m_layoutList.size() - m_itemList.size()); i++)
            {
                ChooseItem *item = new ChooseItem(this);
                m_vLayout->addWidget(item);
                m_itemList.append(item);
            }
        }
        else if (layoutList.size() < m_itemList.size())  //delete
        {
            for (i = 0; i < (m_itemList.size() - layoutList.size()); i++)
            {
                m_itemList.removeLast();
            }
        }

        for (int i = 0; i < layoutList.size(); i++)
        {
            countryName = m_layoutMap.value(layoutList.at(i));
            m_itemList.at(i)->setNames(countryName, layoutList.at(i));
        }
    }
    else
        m_itemList.rem

    //move
}

void LayoutPage::addLayout(QString layoutName)
{
    QDBusPendingReply<> reply = m_keyboardInterface->AddLayout(layoutName);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call AddLayout method failed "
                     << " Error: " << reply.error().message();
        return;
    }
    else
    {
        ChooseItem *item = new ChooseItem(m_layoutMap.value(layoutName),
                                          layoutName,
                                          this);
        m_vLayout->addWidget(item);
        m_itemList.append(item);
        m_layoutList.append(layoutName);
    }
}

void LayoutPage::deleteLayout(QString deletedLayout)
{
    ChooseItem *item = dynamic_cast<ChooseItem *>(sender());
    //界面上删除选择项
    m_itemList.removeOne(item);
    delete item;
    item = nullptr;

    if (m_itemList.isEmpty())
        ui->scrollArea_layout->hide();
    //后端删除该布局
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
