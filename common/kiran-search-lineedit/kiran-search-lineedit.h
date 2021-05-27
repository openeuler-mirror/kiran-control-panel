/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANSEARCHLINEEDIT_H
#define KIRANSEARCHLINEEDIT_H

#include <QLineEdit>

class QListView;
class KiranSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KiranSearchLineEdit(const QStringList &completerKeys, QWidget *parent = nullptr);

private:
    void init();
    void initUI();
    void initConnection();
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void sigSearchConfirmed(const QString &reg);

private slots:
    void handleTextEdited(QString text);
    void handleEditingFinished();

private:
    QStringList m_completerKeys;
    QListView *m_listView;
};

#endif // KIRANSEARCHLINEEDIT_H
