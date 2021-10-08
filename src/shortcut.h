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

class ThreadObject;
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
    void createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo, int type);
    void getAllSystemShortcut();
    void getAllCustomShortcut();

public slots:
    void deleteShortcut(QString uid);
    void editShortcut(QString uid, QString name, QString keyCombination, QString action = nullptr);
    void addShortcut();
    void handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList);

private:
    Ui::Shortcut *ui;
    QList<ShortcutInfo *> m_shortcuts;
    //    QList<ShortcutInfo *> m_customShortcuts;
    QList<ShortcutItem *> m_shortcutItem;
    QToolButton *m_btnModifyApp;
    QToolButton *m_btnCustomApp;
    QThread *m_thread;
    ThreadObject *m_threadObject;

    bool m_isEditMode = false;
};

#endif  // SHORTCUT_H
