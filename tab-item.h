#ifndef TABITEM_H
#define TABITEM_H

#include <QWidget>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class TabItem;
}

class TabItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected NOTIFY isSelectedChanged)
public:
    explicit TabItem(QWidget *parent = nullptr);
    ~TabItem();

    void setText(const QString &text);
    bool isSelected() const;
    const QPixmap *pixmap();
public slots:
    void setSelected(bool isSelected);
signals:
    void isSelectedChanged(bool isSelected);
private:
    Ui::TabItem *ui;
    bool m_isSelected;
    QGraphicsDropShadowEffect* m_effect;
};

#endif // TABITEM_H
