#ifndef KIRANMODECLASSITEM_H
#define KIRANMODECLASSITEM_H

#include <QWidget>

const int cClassItemIconWd = 60;
const int cClassItemPadding = 15;
const int cClassItemTextWd = 100;
const int cClassItemSpace = 9;

namespace Ui {
class KiranModeClassItem;
}

class KiranModeClassItem : public QWidget
{
    Q_OBJECT

public:
    explicit KiranModeClassItem(QWidget *parent = 0);
    ~KiranModeClassItem();

    void setText(const QString &text);
    void setIcon(const QString &icon);
    void setTextVisible(const bool &visible);

    static int textModeWd();
    static int iconModeWd();
    static int heightInt();

private:
    Ui::KiranModeClassItem *ui;
   // void paintEvent(QPaintEvent *);
};

#endif // KIRANMODECLASSITEM_H
