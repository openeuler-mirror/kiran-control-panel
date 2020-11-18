#ifndef KIRANMODECLASSITEMWGT_H
#define KIRANMODECLASSITEMWGT_H

#include <QWidget>
#include <QEvent>

const int cClassItemIconWd = 50;
const int cClassItemPadding = 15;
const int cClassItemTextWd = 100;
const int cClassItemSpace = 9;

class KiranModeClassItemWgt : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModeClassItemWgt(QWidget *parent = nullptr);

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
