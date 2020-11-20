#ifndef KIRANMODECLASSITEMWGT_H
#define KIRANMODECLASSITEMWGT_H

#include <QWidget>
#include <QEvent>

const int cListWidgetPadding = 12;
const int cClassItemIconWd = 40;
const int cClassItemWgtTBPadding = 10;
const int cClassItemWgtLRPadding = 12;
const int cClassItemTextWd = 166;
const int cClassItemSpace = 10;

class KiranModeClassListWidgetItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModeClassListWidgetItemWidget(QWidget *parent = nullptr);

    void setText(const QString &text);

    void setIcon(const QString &icon);
    void setTextVisible(const bool &visible);
    static int textModeWd();
    static int iconModeWd();
    static int heightInt();

    void setChecked(bool checked);

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
    bool m_checked;
};

#endif // KIRANMODECLASSITEMWGT_H
