/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULECLASSLISTWIDGETITEMWIDGET_H
#define KIRANMODULECLASSLISTWIDGETITEMWIDGET_H

#include <QWidget>
#include <QEvent>

const int cListWidgetPadding = 10;
const int cClassItemIconWd = 40;
const int cClassItemWgtTBPadding = 10;
const int cClassItemWgtLRPadding = 12;
const int cClassItemTextWd = 166;
const int cClassItemSpace = 10;
const int cClassItemMargin = 2;

class KiranModuleClassListWidgetItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModuleClassListWidgetItemWidget(QWidget *parent = nullptr);

    void setText(const QString &text);

    void setIcon(const QString &icon);
    void setTextVisible(const bool &visible);
    static int textModeWd();
    static int iconModeWd();
    static int heightInt();

private:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    QPixmap pixmapToRound(const QPixmap &src, const QSize &size, int radius);

private:
    QEvent::Type m_statusType;
    QString m_text;
    QPixmap m_icon;
    QRect m_iconRect;
    QRect m_textRect;
    bool m_showText;
};

#endif // KIRANMODULECLASSLISTWIDGETITEMWIDGET_H
