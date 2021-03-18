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

    painter.save();
    paintProgressBar(painter);
    painter.restore();

    paintCenterPixmap(painter,m_centerPixmap);
}

void EnrollProgressBar::setProgressValue(unsigned int value) {
    m_progressValue = value;
    update();
}

void EnrollProgressBar::paintProgressBar(QPainter &painter) {
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

void EnrollProgressBar::paintCenterPixmap(QPainter& painter, const QPixmap &pixmap) {
    QPoint center = this->rect().center();
    qreal imageRadius = getCenterImageRadius();
    QPainterPath painterPath;
    painterPath.addEllipse(center.x()-imageRadius,center.y()-imageRadius,imageRadius*2,imageRadius*2);
    painter.setClipPath(painterPath);
    painter.drawPixmap( center.x()-(m_centerPixmap.size().width()/2),
                        center.y()-(m_centerPixmap.size().height()/2),
                        m_centerPixmap.size().width(),
                        m_centerPixmap.size().height(),
                        m_centerPixmap);
}

qreal EnrollProgressBar::getCenterImageRadius() {
    qreal  imageRadius = (height()/2.0-m_borerMargin-m_paintProgressLength);
    return imageRadius;
}

void EnrollProgressBar::updateCenterImage(const QPixmap &centerPixmap) {
    m_centerPixmap = centerPixmap;
    update();
}
