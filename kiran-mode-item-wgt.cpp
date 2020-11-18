#include "kiran-mode-item-wgt.h"
#include "ui_kiran-mode-item-wgt.h"
#include <QDebug>

KiranModeItemWgt::KiranModeItemWgt(QWidget *parent) : QWidget(parent), ui(new Ui::KiranModeItemWgt), m_curCenterWgt(nullptr)
{
    ui->setupUi(this);
}

KiranModeItemWgt::~KiranModeItemWgt()
{
    if(m_curCenterWgt) m_curCenterWgt->deleteLater();
    delete ui;
}

void KiranModeItemWgt::setLeftContentsMargins(const int &leftmargin)
{
    ui->horizontalLayout->setContentsMargins(leftmargin, 0, 0, 0);
}

void KiranModeItemWgt::setData(QMap<int, ModelItem> &data)
{
    //    if(data.isEmpty() || (data.count()==1 && data.first()))
    //    {

    //    }
    QMapIterator<int, ModelItem> i(data);
    while (i.hasNext()) {
        i.next();
        ModelItem &modeItem = data[i.key()];
        QStringList names = modeItem.subNameList;
        QStringList icons = modeItem.subIconList;
        int count = names.count();
        for(int i=0; i<count; ++i)
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(names.at(i));
            item->setIcon(QIcon(icons.at(i)));
            item->setData(Qt::UserRole, QVariant::fromValue((void *) &modeItem));
            item->setData(Qt::UserRole+1, names.at(i));
            item->setToolTip(modeItem.commentF());
            ui->listWidget_item->addItem(item);
            modeItem.subItemList << item;
        }
    }
}

void KiranModeItemWgt::setCurItem( QListWidgetItem *item)
{
    ui->listWidget_item->setCurrentItem(item);
}

void KiranModeItemWgt::onClassItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    ui->listWidget_item->clear();

    ModelClass *modelClass =  (ModelClass *) current->data(Qt::UserRole).value<void *>();

    QMap<int, ModelItem> &modeItemMap = modelClass->itemMap;
    setData(modeItemMap);
}

#include <QMessageBox>
void KiranModeItemWgt::on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(previous && m_curCenterWgt)
    {
        ModelItem *preModeItem = (ModelItem *)previous->data(Qt::UserRole).value<void *>();
        if(preModeItem->hasUnsavedOptions())
        {
            QMessageBox::warning(this, "警告", "当前有没有保存，要切换吗");
        }
        m_curCenterWgt->deleteLater();
        m_curCenterWgt = nullptr;
        preModeItem->closePlugin();
    }

    if(!current) return;

    ModelItem *modeItem = (ModelItem *)current->data(Qt::UserRole).value<void *>();
    QString name = current->data(Qt::UserRole+1).toString();

    QWidget *wgt = modeItem->createModeItemSubWgt(name);
    ui->centerLayout->addWidget(wgt);
    m_curCenterWgt = wgt;
}
