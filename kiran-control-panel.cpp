#include "kiran-control-panel.h"
#include "ui_kiran-control-panel.h"
#include "kiran-mode-class-wgt.h"
#include <QListWidgetItem>

KiranControlPanel::KiranControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranControlPanel)
{
    ui->setupUi(this);

    m_classWgt = new KiranModeClassWgt(this);
    connect(m_classWgt, &KiranModeClassWgt::currentItemChanged, ui->widget_itemWgt, &KiranModeItemWgt::onClassItemChanged);
    m_classWgt->move(0, 0);

    m_data = getModeCLass();
    m_classWgt->setData(&m_data);
    ui->widget_itemWgt->setLeftContentsMargins(m_classWgt->iconModeWd());
}

KiranControlPanel::~KiranControlPanel()
{
    delete ui;
}

QStringList KiranControlPanel::completerKeys()
{
    QStringList ret;
    QMapIterator<int, ModelClass> i(m_data);
    while (i.hasNext()) {
        i.next();
        ret << i.value().keywords << i.value().itemKeys();
    }
    return ret;
}

void KiranControlPanel::onSearch(const QString &request)
{
    int row = 0;
    QMapIterator<int, ModelClass> i(m_data);
    while (i.hasNext()) {
        i.next();
        if(i.value().keywords.contains(request))
        {
            m_classWgt->setCurrentItem(i.value().item);
            ui->widget_itemWgt->setCurItem(nullptr);
        }
        else if(i.value().itemKeys().contains(request))
        {
            QListWidgetItem *item = m_data[i.key()].completeredItem(request);
            if(!item) return;

            m_classWgt->setCurrentItem(i.value().item);
            ui->widget_itemWgt->setCurItem(item);
        }
        ++row;
    }
}

void KiranControlPanel::resizeEvent(QResizeEvent *event)
{
    m_classWgt->resize(m_classWgt->width(), this->height());
    QWidget::resizeEvent(event);
}

