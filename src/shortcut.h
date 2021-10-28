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
    ShortcutInfo *getShortcut(QString uid, QString kind);
    bool isConflict(QString &originName, QString newKeyCombination);
    bool isValidKeycode(QList<int> keycodes);
    QString convertToString(QList<int> keyCode);
    QString convertToBackendStr(QString keyStr);
    QString convertToBackendStr(QList<int> keyCode);
    bool getExecFromDesktop(QString fileName, QString &exec);
    void updateShorcut(ShortcutInfo *newShortcut);
    void insertShortcut(ShortcutInfo *shortcutInfo);
    void clearFilterItems();
    int getJsonValue(QString result, QMap<QString, QString> &info);

public slots:
    void addShortcut(QString result);
    void deleteShortcut(QString result);
    void editShortcut(QString result);
    void handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList);
    void handleInputKeycode(QList<int> keycodes);
    void handleAddNewShortcut();
    void handleDeleteShortcut(QString uid);
    void handleEditShortcut(int type, QString uid, QString name, QString keyCombination, QString action = nullptr);
    void openFileSys();
    void search();

private:
    virtual bool eventFilter(QObject *target, QEvent *event);

signals:
    void sigShortcutChanged(QString uid, QString kind);

private:
    Ui::Shortcut *ui;
    QList<ShortcutInfo *> m_shortcuts;
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
