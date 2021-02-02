#ifndef KIRANDISPLAYCONFIGIDENTIFYING_H
#define KIRANDISPLAYCONFIGIDENTIFYING_H

#include <QWidget>

class QEventLoop;
class  KiranDisplayConfigIdentifying : public QWidget
{
    Q_OBJECT
public:
    explicit KiranDisplayConfigIdentifying(QWidget *parent = 0);
    ~KiranDisplayConfigIdentifying();

    void exec(const QPoint &pos);

    void setText(const QString &text);

private:
    void hideEvent(QHideEvent *event);
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    QEventLoop *m_loop;
    QString m_text;
};


#endif // KIRANDISPLAYCONFIGIDENTIFYING_H
