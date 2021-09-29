#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <keybinding_def.h>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
class Shortcut;
}

Q_DECLARE_METATYPE(ShortcutInfo *)

class ShortcutItem;
class Shortcut : public QWidget
{
    Q_OBJECT

public:
    explicit Shortcut(QWidget *parent = 0);
    ~Shortcut();
    QSize sizeHint() const override;

private:
    void initUI();
    void createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo);
    void getAllSystemShortcut();
    void getAllCustomShortcut();
    void getJsonValueFromString(QString jsonString, int type);

public slots:
    void deleteShortcut(QString uid);
    void editShortcut(QString uid, QString name, QString keyCombination, QString action = nullptr);
    void addShortcut();

private:
    Ui::Shortcut *ui;
    QList<ShortcutInfo *> m_systemShortcuts;
    QList<ShortcutInfo *> m_customShortcuts;
    QList<ShortcutItem *> m_shortcutItem;
    QToolButton *m_btnModifyApp;
    QToolButton *m_btnCustomApp;

    bool m_isEditMode = false;
};

#endif  // SHORTCUT_H
