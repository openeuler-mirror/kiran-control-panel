#include "kiran-time-picker-widget.h"
#include "ui_kiran-time-picker-widget.h"

#include <kiran-palette.h>

#include <QStyleOption>
#include <QPainter>
#include <QDebug>

KiranTimePickerWidget::KiranTimePickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimePickerWidget),
    m_pickerType(PICKER_HOUR_MINUTE_SECOND),
    m_hightLightColor(QColor(255,255,255,255*0.05))
{
    ui->setupUi(this);
    initUI();
}

KiranTimePickerWidget::KiranTimePickerWidget(KiranTimePickerWidget::TimePickerType type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimePickerWidget),
    m_pickerType(PICKER_HOUR_MINUTE),
    m_hightLightColor(QColor(255,255,255,255*0.05))
{
    ui->setupUi(this);
    initUI();
}

KiranTimePickerWidget::~KiranTimePickerWidget()
{
    delete ui;
}

void KiranTimePickerWidget::setPickerType(KiranTimePickerWidget::TimePickerType pickerType)
{
    m_pickerType = pickerType;
    switch (pickerType) {
    case PICKER_HOUR_MINUTE:
        ui->scrollpicker_second->setVisible(false);
        break;
    case PICKER_HOUR_MINUTE_SECOND:
        ui->scrollpicker_second->setVisible(true);
        break;
    default:
        qFatal("error TimePickerType");
    }
}

QTime KiranTimePickerWidget::currentTime()
{
    QTime time;

    int hour = ui->scrollpicker_hour->currentData().toInt();
    int minute = ui->scrollpicker_minute->currentData().toInt();
    int second = ui->scrollpicker_second->currentData().toInt();

    time.setHMS(hour,minute,second);

    return time;
}

void KiranTimePickerWidget::setCurrentTime(const QTime &time)
{
    ui->scrollpicker_hour->setCurrentText(QString("%1").arg(time.hour(),2,10,QChar('0')),true);
    ui->scrollpicker_minute->setCurrentText(QString("%1").arg(time.minute(),2,10,QChar('0')),true);
    ui->scrollpicker_second->setCurrentText(QString("%1").arg(time.second(),2,10,QChar('0')),true);
}

QColor KiranTimePickerWidget::hightLightColor() const
{
    return m_hightLightColor;
}

void KiranTimePickerWidget::reset()
{
    setCurrentTime(QTime::currentTime());
}

void KiranTimePickerWidget::setHightLightColor(QColor hightLightColor)
{
    if(hightLightColor==m_hightLightColor){
        return;
    }
    m_hightLightColor = hightLightColor;
    update();
}

void KiranTimePickerWidget::initUI()
{
    ui->scrollpicker_hour->setShowCount(3);
    ui->scrollpicker_hour->setLoop(true);
    for(int i=0;i<24;i++){
        ui->scrollpicker_hour->addItem(QString("%1").arg(i,2,10,QChar('0')),i);
    }

    ui->scrollpicker_minute->setShowCount(3);
    ui->scrollpicker_minute->setLoop(true);
    for(int i=0;i<60;i++){
        ui->scrollpicker_minute->addItem(QString("%1").arg(i,2,10,QChar('0')),i);
    }

    ui->scrollpicker_second->setShowCount(3);
    ui->scrollpicker_second->setLoop(true);
    for(int i=0;i<60;i++){
        ui->scrollpicker_second->addItem(QString("%1").arg(i,2,10,QChar('0')),i);
    }

    setPickerType(m_pickerType);

    setCurrentTime(QTime::currentTime());

    updateScrollPickerColor();
    connect(KiranPalette::instance(),&KiranPalette::themeChanged,this,&KiranTimePickerWidget::updateScrollPickerColor);

    connect(ui->scrollpicker_hour,QOverload<const QString&>::of(&ScrollPicker::currentTextChanged),[this](){
        emitCurrentTimeChanged();
    });
    connect(ui->scrollpicker_minute,QOverload<const QString&>::of(&ScrollPicker::currentTextChanged),[this](){
        emitCurrentTimeChanged();
    });
    connect(ui->scrollpicker_second,QOverload<const QString&>::of(&ScrollPicker::currentTextChanged),[this](){
        emitCurrentTimeChanged();
    });
}

void KiranTimePickerWidget::emitCurrentTimeChanged()
{
    QTime time;
    time = currentTime();
    emit currentTimeChanged(time);
}

void KiranTimePickerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    ///绘制高亮区域
    QRect hightLightRect(QPoint(0,height()/2-20),QSize(width(),40));
    painter.fillRect(hightLightRect,m_hightLightColor);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QWidget::paintEvent(event);
}

void KiranTimePickerWidget::updateScrollPickerColor()
{
    auto kiranPalette = KiranPalette::instance();
    QColor fontColor = kiranPalette->color(KiranPalette::Normal,KiranPalette::Widget,KiranPalette::Foreground);

    ui->scrollpicker_hour->setFontColor(fontColor);
    ui->scrollpicker_minute->setFontColor(fontColor);
    ui->scrollpicker_second->setFontColor(fontColor);
}
