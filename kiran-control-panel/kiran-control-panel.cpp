#include "kiran-control-panel.h"
#include "ui_kiran-control-panel.h"
#include "kiran-module-class-listwidget.h"
#include <QTimer>
#include <QListWidgetItem>

KiranControlPanel::KiranControlPanel(QWidget *parent) :
    QWidget(parent), m_curClassListItem(nullptr),
    ui(new Ui::KiranControlPanel)
{
    ui->setupUi(this);

    m_classWgt = new KiranModuleClassListWidget(this);
    m_classWgt->move(0, 0);
    //使信号槽绑定在m_classWgt->setData(&m_data)之前生效。
    connect(m_classWgt, &KiranModuleClassListWidget::currentItemChanged, this, &KiranControlPanel::onCurrentItemChanged);

    m_data = getModuleCLass();
    m_classWgt->setData(&m_data);
    ui->module_widget->setLeftContentsMargins(m_classWgt->iconModeWd());
}

void KiranControlPanel::onSearch(const QString &request)
{
    int row = 0;
    QMapIterator<int, ModuleClass> i(m_data);
    while (i.hasNext()) {
        i.next();
        //如果与模块类的关键字匹配.模块类关键字与模块关键字分开存储.
        if(i.value().keywords.contains(request))
        {
            m_classWgt->setCurrentRow(i.value().row);
            break;
        }
        else if(i.value().itemKeys().contains(request))
        {
            int row = m_data[i.key()].completeredItemRow(request);

            ui->module_widget->setDefaultSelectFirstItem(false);
            m_classWgt->setCurrentRow(i.value().row);
            ui->module_widget->setCurModuleSubRow(row);
            ui->module_widget->setDefaultSelectFirstItem(true);
            break;
        }
        ++row;
    }
}

KiranControlPanel::~KiranControlPanel()
{
    delete ui;
}

QStringList KiranControlPanel::completerKeys()
{
    QStringList ret;
    QMapIterator<int, ModuleClass> i(m_data);
    while (i.hasNext()) {
        i.next();
        ret << i.value().keywords << i.value().itemKeys();
    }
    return ret;
}

void KiranControlPanel::resizeEvent(QResizeEvent *event)
{
    m_classWgt->resize(m_classWgt->width(), this->height());
    QWidget::resizeEvent(event);
}

void KiranControlPanel::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    QTimer::singleShot(5, this, [=](){
        if(!ui->module_widget->checkHasUnSaved())
        {
            QTimer::singleShot(5, this, [=](){
                m_classWgt->blockSignals(true);
                m_classWgt->setCurrentItem(m_curClassListItem);
                m_classWgt->blockSignals(false);
            });

            return;
        }
        m_curClassListItem = current;
        int row = m_classWgt->row(current);
        ModuleClass *moduleClass = getMapValueByRow<ModuleClass>(row, m_data);
        if(!moduleClass) return;

        QMap<int, ModuleItem> &moduleItemMap = moduleClass->itemMap;

        ui->module_widget->onSelectedClassItemChanged(&moduleItemMap);
    });
}

