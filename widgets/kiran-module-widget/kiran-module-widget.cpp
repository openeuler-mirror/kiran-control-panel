/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-module-widget.h"
#include "ui_kiran-module-widget.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

KiranModuleWidget::KiranModuleWidget(QWidget *parent) : QWidget(parent), ui(new Ui::KiranModuleWidget), m_curItem(nullptr), m_curCenterWgt(nullptr),m_retracement(false)
{
    ui->setupUi(this);
}

KiranModuleWidget::~KiranModuleWidget()
{
    if(m_curCenterWgt)
    {
        closeCenterWidgetPlugin(ui->listWidget_item->currentItem());
    }
    delete ui;
}

void KiranModuleWidget::setLeftContentsMargins(const int &leftmargin)
{
    ui->horizontalLayout->setContentsMargins(leftmargin, 0, 0, 0);
}

void KiranModuleWidget::setModulesData(QMap<int, ModuleItem> &data)
{
    if(ui->listWidget_item->isHidden()) ui->listWidget_item->show();
    //当分类下模块数量为0时.
    if(data.count() == 0)
    {
        ui->listWidget_item->hide();
        return;
    }

    QMapIterator<int, ModuleItem> i(data);
    while (i.hasNext()) {
        i.next();
        ModuleItem &moduleItem = data[i.key()];
        QStringList names = moduleItem.subNameList;
        QStringList icons = moduleItem.subIconList;
        int count = names.count();
        //当模块数量为1,且模块中的功能项目小于等于1时.
        if(data.count()==1 && count <= 1)
        {
            ui->listWidget_item->hide();
            continue;
        }
        else if(count == 0)//当模块数量大于1,而某个模块的功能项为0时.
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(moduleItem.getNameTranslate());
            item->setIcon(QIcon(moduleItem.icon));
            item->setData(Qt::UserRole, QVariant::fromValue((void *) &moduleItem));
            item->setData(Qt::UserRole+1, moduleItem.name);
            item->setToolTip(moduleItem.getCommentTranslate());
            ui->listWidget_item->addItem(item);
            moduleItem.subItemList << item;
            continue;
        }

        for(int i=0; i<count; ++i)
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(names.at(i));
            item->setIcon(QIcon(icons.at(i)));
            item->setData(Qt::UserRole, QVariant::fromValue((void *) &moduleItem));
            item->setData(Qt::UserRole+1, names.at(i));
            item->setToolTip(moduleItem.getCommentTranslate());
            ui->listWidget_item->addItem(item);
            moduleItem.subItemList << item;
        }
    }
    //加载完数据后,默认选中第一行.
    if(ui->listWidget_item->count() <= 0) return;
    ui->listWidget_item->setCurrentRow(0);
}

void KiranModuleWidget::setCurModuleSubItem( QListWidgetItem *item)
{
    ui->listWidget_item->setCurrentItem(item);
}

bool KiranModuleWidget::checkHasUnSaved()
{
    if(m_curItem && m_curCenterWgt)
    {
        ModuleItem *preModuleItem = (ModuleItem *)m_curItem->data(Qt::UserRole).value<void *>();
        if(preModuleItem->hasUnsavedOptions())
        {
            KiranMessageBox box;
            box.setTitle(tr("警告"));

            QPushButton saveBtn;
            saveBtn.setText(tr("确定(K)"));
            saveBtn.setFixedSize(QSize(200, box.buttonSize().height()));

            QPushButton cancelBtn;
            cancelBtn.setText(tr("取消(R)"));
            cancelBtn.setFixedSize(QSize(200, box.buttonSize().height()));

            box.addButton(&saveBtn, QDialogButtonBox::AcceptRole);
            box.addButton(&cancelBtn, QDialogButtonBox::RejectRole);
            saveBtn.setShortcut(Qt::CTRL + Qt::Key_K);
            cancelBtn.setShortcut(Qt::CTRL + Qt::Key_R);
            box.setText(tr("当前页面未保存，确定要切换吗？"));
            box.exec();
            if( box.clickedButton() == &saveBtn  )
            {
            }
            else
            {
                QTimer::singleShot(5, this, [=](){
                    ui->listWidget_item->blockSignals(true);
                    ui->listWidget_item->setCurrentItem(m_curItem);
                    ui->listWidget_item->blockSignals(false);
                });

                return false;
            }
        }
    }

    return true;
}

void KiranModuleWidget::onSelectedClassItemChanged(QListWidgetItem *current)
{
    closeCenterWidgetPlugin(ui->listWidget_item->currentItem());
    ui->listWidget_item->clear();

    ModuleClass *moduleClass =  (ModuleClass *) current->data(Qt::UserRole).value<void *>();

    QMap<int, ModuleItem> &moduleItemMap = moduleClass->itemMap;
    setModulesData(moduleItemMap);
}


void KiranModuleWidget::changeCurModuleSubItem(QListWidgetItem *current)
{
    if(!checkHasUnSaved()) return;

    if(m_curItem && m_curCenterWgt)
    {
        closeCenterWidgetPlugin(m_curItem);
    }

    if(!current) return;

    ModuleItem *moduleItem = (ModuleItem *)current->data(Qt::UserRole).value<void *>();
    QString name = current->data(Qt::UserRole+1).toString();

    QWidget *wgt = moduleItem->createModuleItemSubWgt(name);
    if(!wgt) return;
    ui->centerLayout->addWidget(wgt);
    m_curItem = current;
    m_curCenterWgt = wgt;
}

void KiranModuleWidget::closeCenterWidgetPlugin(QListWidgetItem *current)
{
    if(!current || !m_curCenterWgt) return;

    ModuleItem *preModuleItem = (ModuleItem *)current->data(Qt::UserRole).value<void *>();
    m_curCenterWgt->deleteLater();
    m_curCenterWgt = nullptr;
    m_curItem = nullptr;
    //其它属性值从插件中获取之后，可以立即关闭插件。QWidget在使用完之后再关闭插件。
    preModuleItem->closePlugin();
}

bool KiranModuleWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if(m_curItem && ui->listWidget_item->itemAt(e->pos()) != m_curItem)
        {
            if(!checkHasUnSaved())
            {
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

void KiranModuleWidget::on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    changeCurModuleSubItem(current);
}
