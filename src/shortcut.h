#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <keybinding_def.h>
#include <QFile>
#include <QLineEdit>
#include <QTimer>
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
class KeybindingBackEndProxy;
class Shortcut : public QWidget
{
    Q_OBJECT

public:
    explicit Shortcut(QWidget *parent = 0);
    ~Shortcut();
    QSize sizeHint() const override;

private:
    void initUI();
    ShortcutItem *createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo, int type);
    void getAllShortcuts();
    bool isConflict(QString &originName, QString newKeyCombination);
    bool isValidKeycode(QList<int> keycodes);
    QString convertToString(QList<int> keyCode);
    QString convertToBackendStr(QString keyStr);
    bool getExecFromDesktop(QString fileName, QString &exec);
    void updateShorcut(QString uid, QString name, QString action, QString keyCombination);
    void updateShorcut(QString uid, QString keyCombination);
    void clearFilterItems();
    void connectDbusSignal();

public slots:
    void addShortcut();
    void handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList);
    void handleInputKeycode(QList<int> keycodes);
    void handleAddNewShortcut();
    void handleDeleteShortcut(QString uid);
    void handleEditShortcut(int type, QString uid, QString name, QString keyCombination, QString action = nullptr);
    void handleDbusChanges(QString result);
    void openFileSys();
    void search();

private:
    virtual bool eventFilter(QObject *target, QEvent *event);

signals:
    void sigShortcutChanged(QString uid, QString kind);

private:
    Ui::Shortcut *ui;
    QList<ShortcutInfo *> m_shortcuts;
    //    QList<ShortcutInfo *> m_customShortcuts;
    QList<ShortcutItem *> m_shortcutItem;
    QList<ShortcutItem *> m_filterItem;
    QToolButton *m_btnModifyApp;
    QToolButton *m_btnCustomApp;
    CustomLineEdit *m_lECustomKey;
    CustomLineEdit *m_lEModifyKey;
    QThread *m_thread;
    ThreadObject *m_threadObject;
    KeyMap *m_keyMap;
    KeybindingBackEndProxy *m_keybindingInterface;
    QTimer *m_timer;
    bool m_isEditMode = false;
    int m_customShortcutCount = 0;
};

#endif  // SHORTCUT_H
