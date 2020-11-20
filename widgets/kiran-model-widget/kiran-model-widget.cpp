#include "kiran-model-widget.h"
#include "ui_kiran-model-widget.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

KiranModelWidget::KiranModelWidget(QWidget *parent) : QWidget(parent), ui(new Ui::KiranModelWidget), m_curItem(nullptr), m_curCenterWgt(nullptr),m_retracement(false)
{
    ui->setupUi(this);
  //  ui->listWidget_item->viewport()->installEventFilter(this);
}

KiranModelWidget::~KiranModelWidget()
{
    if(m_curCenterWgt)
    {
        closeCenterWidgetPlugin(ui->listWidget_item->currentItem());
    }
    delete ui;
}

void KiranModelWidget::setLeftContentsMargins(const int &leftmargin)
{
    ui->horizontalLayout->setContentsMargins(leftmargin, 0, 0, 0);
}

void KiranModelWidget::setModelsData(QMap<int, ModelItem> &data)
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
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
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

void KiranModelWidget::setCurModelSubItem( QListWidgetItem *item)
{
    ui->listWidget_item->setCurrentItem(item);
}

bool KiranModelWidget::checkHasUnSaved()
{
    if(m_curItem && m_curCenterWgt)
    {
        ModelItem *preModeItem = (ModelItem *)m_curItem->data(Qt::UserRole).value<void *>();
        if(preModeItem->hasUnsavedOptions())
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

void KiranModelWidget::onSelectedClassItemChanged(QListWidgetItem *current)
{
    closeCenterWidgetPlugin(ui->listWidget_item->currentItem());
    ui->listWidget_item->clear();

    ModelClass *modelClass =  (ModelClass *) current->data(Qt::UserRole).value<void *>();

    QMap<int, ModelItem> &modeItemMap = modelClass->itemMap;
    setModelsData(modeItemMap);
}


void KiranModelWidget::changeCurModelSubItem(QListWidgetItem *current)
{
    if(!checkHasUnSaved()) return;

    if(m_curItem && m_curCenterWgt)
    {
        closeCenterWidgetPlugin(m_curItem);
    }

    if(!current) return;

    ModelItem *modeItem = (ModelItem *)current->data(Qt::UserRole).value<void *>();
    QString name = current->data(Qt::UserRole+1).toString();

    QWidget *wgt = modeItem->createModeItemSubWgt(name);
    if(!wgt) return;
    ui->centerLayout->addWidget(wgt);
    m_curItem = current;
    m_curCenterWgt = wgt;
}

void KiranModelWidget::closeCenterWidgetPlugin(QListWidgetItem *current)
{
    if(!current || !m_curCenterWgt) return;

    ModelItem *preModeItem = (ModelItem *)current->data(Qt::UserRole).value<void *>();
    m_curCenterWgt->deleteLater();
    m_curCenterWgt = nullptr;
    m_curItem = nullptr;
    //其它属性值从插件中获取之后，可以立即关闭插件。QWidget在使用完之后再关闭插件。
    preModeItem->closePlugin();
}

bool KiranModelWidget::eventFilter(QObject *obj, QEvent *event)
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

void KiranModelWidget::on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    changeCurModelSubItem(current);
}
