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
class KeyMap;
class CustomLineEdit;
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
    void getAllShortcuts();
    bool isConflict(QString keyStr);
    QString convertToString(QList<int> keyCode);

public slots:
    void deleteShortcut(QString uid);
    void editShortcut(QString uid, QString name, QString keyCombination, QString action = nullptr);
    void addShortcut();
    void handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList);
    void handleInputKeycode(QList<int> keycodes);

private:
    virtual bool eventFilter(QObject *target, QEvent *event);

private:
    Ui::Shortcut *ui;
    QList<ShortcutInfo *> m_shortcuts;
    //    QList<ShortcutInfo *> m_customShortcuts;
    QList<ShortcutItem *> m_shortcutItem;
    QToolButton *m_btnModifyApp;
    QToolButton *m_btnCustomApp;
    CustomLineEdit *m_lECustomKey;
    CustomLineEdit *m_lEModifyKey;
    QThread *m_thread;
    ThreadObject *m_threadObject;
    KeyMap *m_keyMap;
    bool m_isEditMode = false;
};

#endif  // SHORTCUT_H
