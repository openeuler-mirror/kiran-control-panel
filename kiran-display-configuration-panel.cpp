#include "kiran-display-configuration-panel.h"
#include "ui_kiran-display-configuration-panel.h"
#include "kiran-display-config-global.h"
#include "kiran-display-config-identifying.h"
#include<QToolTip>
#include <QPainter>
#include <QDebug>

using namespace KiranDisplayConfigGlobal;

KiranDisplayConfigurationPanel::KiranDisplayConfigurationPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::KiranDisplayConfigurationPanel)
{
    ui->setupUi(this);

    connect(ui->contain, &KiranDisplayConfigItemContain::buttonChecked, this, &KiranDisplayConfigurationPanel::buttonChecked);
}

KiranDisplayConfigurationPanel::~KiranDisplayConfigurationPanel()
{
    delete ui;
}

QVariantMap KiranDisplayConfigurationPanel::getData()
{
    return ui->contain->getData();
}

void KiranDisplayConfigurationPanel::setData(const QVariantList &list, const bool &clearChecked)
{
    ui->pushButton_identifying->setVisible(list.count() > 1);
    ui->contain->setData(list, clearChecked);
    m_textList = list;
    m_textSize.clear();
}

void KiranDisplayConfigurationPanel::paintEvent(QPaintEvent *event)
{
    if(m_textSize.isEmpty())
    {
        QPainter painter(this);
        painter.save();
        QFont pen = painter.font();
        pen.setPixelSize(64);
        QFontMetrics fm = painter.fontMetrics();
        foreach (QVariant var, m_textList) {
            QString text = var.toMap().value("text").toString();
            m_textSize.insert(text, QSize(fm.width(text), fm.height()));
        }
        painter.restore();
    }

    QFrame::paintEvent(event);
}

void KiranDisplayConfigurationPanel::on_pushButton_left_clicked()
{
    ui->contain->setRotateDrect(KiranDisplayConfigItem::Left);
}

void KiranDisplayConfigurationPanel::on_pushButton_vertical_clicked()
{
    ui->contain->setRotateDrect(KiranDisplayConfigItem::Inverted);
}

void KiranDisplayConfigurationPanel::on_pushButton_right_clicked()
{
    ui->contain->setRotateDrect(KiranDisplayConfigItem::Right);
}

void KiranDisplayConfigurationPanel::on_pushButton_identifying_clicked()
{
    QString monitorPath = ui->contain->getCurMonitorPath();
    if(monitorPath.isEmpty()) return;
    QString text = ui->contain->getCurMonitorText();
    QPoint pos = QPoint(MonitorProperty(monitorPath, "x").toInt(), MonitorProperty(monitorPath, "y").toInt());

    KiranDisplayConfigIdentifying identify(this);
    identify.setText(text);
    identify.exec(pos);
}
