#ifndef KIRANCOMBOBOX_H
#define KIRANCOMBOBOX_H

#include <QComboBox>

class QListView;
class KiranComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit KiranComboBox(QWidget *parent = nullptr);

signals:

public slots:

private:
    bool eventFilter(QObject *obj, QEvent *event);
    QListView *m_listView;
};

#endif // KIRANCOMBOBOX_H
