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
    connect(m_classWgt, &KiranModuleClassListWidget::currentItemChanged, this, [=](QListWidgetItem *current, QListWidgetItem*){
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
        ui->module_widget->onSelectedClassItemChanged(current);
    });

    m_classWgt->move(0, 0);

    m_data = getModuleCLass();
    m_classWgt->setData(&m_data);
    ui->module_widget->setLeftContentsMargins(m_classWgt->iconModeWd());
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

void KiranControlPanel::onSearch(const QString &request)
{
    int row = 0;
    QMapIterator<int, ModuleClass> i(m_data);
    while (i.hasNext()) {
        i.next();
        //如果与模块类的关键字匹配.模块类关键字与模块关键字分开存储.
        if(i.value().keywords.contains(request))
        {
            m_classWgt->setCurrentItem(i.value().item);
        }
        else if(i.value().itemKeys().contains(request))
        {
            QListWidgetItem *item = m_data[i.key()].completeredItem(request);
            if(!item) return;

            m_classWgt->setCurrentItem(i.value().item);
            ui->module_widget->setCurModuleSubItem(item);
        }
        ++row;
    }
}

void KiranControlPanel::resizeEvent(QResizeEvent *event)
{
    m_classWgt->resize(m_classWgt->width(), this->height());
    QWidget::resizeEvent(event);
}

