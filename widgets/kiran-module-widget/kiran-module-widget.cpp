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

#define MODULE_STU_POINTER Qt::UserRole
#define FUNCTION_NAME Qt::UserRole +1
#define FUNCTION_KEY Qt::UserRole + 2


KiranModuleWidget::KiranModuleWidget(QWidget *parent) : QWidget(parent), ui(new Ui::KiranModuleWidget), m_curItem(nullptr), m_curCenterWidget(nullptr),m_defaultSelectFirstItem(true)
{
    ui->setupUi(this);
    ui->listWidget_module->setGridSize(QSize(296, 84));//设置item的总占用大小,包括间隙.
    ui->listWidget_module->setIconSize(QSize(15, 15));
}

KiranModuleWidget::~KiranModuleWidget()
{
    if(m_curCenterWidget)
    {
        //模块窗口释放表示程序退出了。
        //窗口关闭时,右侧的插件实例对象可能没有delete,同时插件可能没有dlclose.这里关闭插件,释放实例.
        //closeCenterWidgetPlugin(ui->listWidget_module->currentItem());
    }
    delete ui;
}

void KiranModuleWidget::setLeftContentsMargins(const int &leftmargin)
{
    ui->horizontalLayout->setContentsMargins(leftmargin, 0, 0, 0);
}

void KiranModuleWidget::setData(QMap<int, ModuleItem> *data)
{
    if(ui->listWidget_module->isHidden()) ui->listWidget_module->show();
    //当分类下模块数量为0时.
    if(data->count() == 0)
    {
        ui->listWidget_module->hide();
        return;
    }

    QMapIterator<int, ModuleItem> i(*data);
    while (i.hasNext()) {
        i.next();
        ModuleItem &moduleItem = (*data)[i.key()];
        int count = moduleItem.subItems.count();
        //当模块数量为1,且模块中的功能项目小于等于1时.
        if(data->count()==1 && count <= 1)
        {
            ui->listWidget_module->hide();
        }
        else if(count == 0)//当模块数量大于1,而某个模块的功能项为0时.
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(moduleItem.getNameTranslate());
            item->setIcon(QIcon(moduleItem.icon));
            item->setData(MODULE_STU_POINTER, QVariant::fromValue(&(*data)[i.key()]));
            item->setData(FUNCTION_NAME, moduleItem.name);//当功能项为空时，使用模块名，实际上都是创建不了窗口对象的。
            item->setData(FUNCTION_KEY, moduleItem.name);
            item->setToolTip(moduleItem.getCommentTranslate());
            ui->listWidget_module->addItem(item);
            continue;
        }

        for(int j=0; j<count; ++j)
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(moduleItem.subItems.at(j).name);
            item->setIcon(QIcon(moduleItem.subItems.at(j).icon));
            item->setData(MODULE_STU_POINTER, QVariant::fromValue(&(*data)[i.key()]));//item中保存数据机构体指针,数据结构体中保存item指针.双向绑定,方便寻址减少计算量.
            item->setData(FUNCTION_NAME, moduleItem.subItems.at(j).name);//存储功能项的名称,当切换为此item时,通过功能项名称获取功能项实例.
            item->setData(FUNCTION_KEY, moduleItem.subItems.at(j).key);
            item->setToolTip(moduleItem.getCommentTranslate());
            ui->listWidget_module->addItem(item);
        }
    }
    //加载完数据后,默认选中第一行.
    if(m_defaultSelectFirstItem && ui->listWidget_module->count() > 0)
        ui->listWidget_module->setCurrentRow(0);
}
/*!
 * \brief KiranModuleWidget::setModuleCurSubItem 多个模块的功能项合并，不能使用数据结构体中的row选取listWidget中的row.
 * \param functionName
 */
void KiranModuleWidget::setModuleCurSubItem(const QString &functionName)
{
    int count = ui->listWidget_module->count();
    for(int i=0; i<count; ++i)
    {
        QString f_name = ui->listWidget_module->item(i)->data(FUNCTION_KEY).toString();
        if(f_name == functionName)
        {
            ui->listWidget_module->setCurrentRow(i);
            return;
        }
    }
}

void KiranModuleWidget::setCurModuleSubRow(const int &row)
{
    ui->listWidget_module->setCurrentRow(row);
}

bool KiranModuleWidget::checkHasUnSaved()
{
    if(m_curItem && m_curCenterWidget)
    {
        ModuleItem *preModuleItem = m_curItem->data(MODULE_STU_POINTER).value<ModuleItem *>();
        if(!preModuleItem) return false;

        if(preModuleItem->hasUnsavedOptions())
        {
            KiranMessageBox box(this);
            box.setTitle(tr("Warning"));

            QPushButton saveBtn;
            saveBtn.setText(tr("OK(K)"));
            saveBtn.setFixedSize(QSize(200, box.buttonSize().height()));

            QPushButton cancelBtn;
            cancelBtn.setText(tr("Cancel(R)"));
            cancelBtn.setFixedSize(QSize(200, box.buttonSize().height()));

            box.addButton(&saveBtn, QDialogButtonBox::AcceptRole);
            box.addButton(&cancelBtn, QDialogButtonBox::RejectRole);
            saveBtn.setShortcut(Qt::CTRL + Qt::Key_K);
            cancelBtn.setShortcut(Qt::CTRL + Qt::Key_R);
            box.setText(tr("The edited content in %1 is not saved. After switching, the edited content will be lost. Are you sure you want to switch？").arg(m_curItem->text()));
            box.exec();
            if( box.clickedButton() == &saveBtn  )
            {
            }
            else
            {
                //此时item的切换信号绑定的槽可能还未全部执行完,等待它们全部执行完成,再开始回撤.
                QTimer::singleShot(5, this, [=](){
                    ui->listWidget_module->blockSignals(true);
                    ui->listWidget_module->setCurrentItem(m_curItem);
                    ui->listWidget_module->blockSignals(false);
                });

                return false;
            }
        }
    }

    return true;
}
void KiranModuleWidget::onSelectedClassItemChanged(QMap<int, ModuleItem> *modules)
{
    closeCenterWidgetPlugin(ui->listWidget_module->currentItem());
    ui->listWidget_module->clear();

    setData(modules);
}
/*!
 * \brief KiranModuleWidget::changeCurModuleSubItem 功能项切换时都通过此函数处理.
 * \param current
 */
void KiranModuleWidget::changeCurModuleSubItem(QListWidgetItem *current)
{
    if(!checkHasUnSaved()) return;

    if(m_curItem && m_curCenterWidget)
    {
        closeCenterWidgetPlugin(m_curItem);
    }

    if(!current) return;

    ModuleItem *moduleItem = current->data(MODULE_STU_POINTER).value<ModuleItem *>();
    if(!moduleItem) return;
    QString name = current->data(FUNCTION_NAME).toString();

    QWidget *wgt = moduleItem->createModuleItemSubWgt(name);
    if(!wgt) return;
    ui->centerLayout->addWidget(wgt);
    m_curItem = current;
    m_curCenterWidget = wgt;
}

void KiranModuleWidget::closeCenterWidgetPlugin(QListWidgetItem *current)
{
    if(!current || !m_curCenterWidget) return;

    ModuleItem *preModuleItem = current->data(MODULE_STU_POINTER).value<ModuleItem *>();
    if(!preModuleItem) return;

    m_curCenterWidget->deleteLater();
    m_curCenterWidget = nullptr;
    m_curItem = nullptr;
    //其它属性值从插件中获取之后，可以立即关闭插件。QWidget在使用完之后再关闭插件。
    preModuleItem->closePlugin();
    preModuleItem->removeTranslator();
}

/*!
 * \brief KiranModuleWidget::eventFilter 这个函数暂时没有用到,作为备注.
 * \param obj
 * \param event
 * \return
 */
bool KiranModuleWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if(m_curItem && ui->listWidget_module->itemAt(e->pos()) != m_curItem)
        {
            if(!checkHasUnSaved())
            {
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
/*!
 * \brief KiranModuleWidget::setDefaultSelectFirstItem 搜索某一功能项时，如果class需要切换，切换后引起module的item重新加载，此时禁止默认选中第一行。后续在将搜索到的功能项设为选中。
 * \param defaultSelectFirstItem
 */
void KiranModuleWidget::setDefaultSelectFirstItem(bool defaultSelectFirstItem)
{
    m_defaultSelectFirstItem = defaultSelectFirstItem;
}

void KiranModuleWidget::on_listWidget_module_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    //让被点击的item先绘制完选中效果,避免回撤后再绘制,出现闪动.
    QTimer::singleShot(5, this, [=](){
        changeCurModuleSubItem(current);
    });
}
