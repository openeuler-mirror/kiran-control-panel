//
// Created by lxh on 2021/3/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_EnrollProgressBar.h" resolved

#include "enroll-progressbar.h"
#include <QPainter>

EnrollProgressBar::EnrollProgressBar(QWidget *parent) :
        QWidget(parent) {
    startTimer(100);
}

EnrollProgressBar::~EnrollProgressBar() {

}

void EnrollProgressBar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    QColor normalColor("#2d2d2d");
    QColor activeColor("#2eb3ff");

    painter.translate(width()/2,height()/2);

    QPen pen;
    pen.setWidthF(1.4);

    qreal rotateAngle = 0;
    for(int i=0;i<m_progressLinesCount;i++){
        qreal percent = rotateAngle/360;
        int iPercent = percent*100;
        if(iPercent < m_progressValue){
            pen.setColor(activeColor);
        }else{
            pen.setColor(normalColor);
        }
        painter.setPen(pen);

        painter.drawLine(0,-(height()/2)+m_borerMargin,0,-(height()/2)+m_paintProgressLength);
        painter.rotate(360.0/m_progressLinesCount);
        rotateAngle += 360.0/m_progressLinesCount;
    }
}

void EnrollProgressBar::timerEvent(QTimerEvent *event) {
    m_progressValue+=1;
    if( m_progressValue>100 ){
        m_progressValue = 0;
    }
    update();
    QObject::timerEvent(event);
}
