/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANSEARCHLINEEDIT_H
#define KIRANSEARCHLINEEDIT_H

#include <QLineEdit>

class KiranSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KiranSearchLineEdit(const QStringList &completerKeys, QWidget *parent = nullptr);

signals:
    void sigSearch(const QString &reg);

public slots:

private:
    QStringList m_completerKeys;
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // KIRANSEARCHLINEEDIT_H
