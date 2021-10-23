#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include <QWidget>
#include "keybinding_def.h"

namespace Ui
{
class ShortcutItem;
}
//Q_DECLARE_METATYPE(ShortcutInfo *)

class ShortcutItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShortcutItem(int type, ShortcutInfo *shortcutInfo, QWidget *parent = 0);
    ~ShortcutItem();
    void setname(QString);
    void setKeyBinding(QString);
    void setAction(QString action);
    void setEditMode(bool isEditMode);
    QString getName();
    int getType();
    QString getUid();
    ShortcutInfo *getShortcut();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initUI();
    QString handleKeyCombination(QString origStr);

signals:
    void sigDelete(QString uid);
    void sigClicked(int type, QString uid, QString name, QString key_combination, QString action);

private:
    Ui::ShortcutItem *ui;
    int m_type;
    ShortcutInfo *m_shortcutInfo;
    bool m_isPressed = false;
};

#endif  // SHORTCUTITEM_H
