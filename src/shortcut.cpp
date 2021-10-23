#include "shortcut.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/keybinding-i.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <QClipboard>
#include <QFileDialog>
#include <QKeyEvent>
#include "custom-line-edit.h"
#include "dbus-wrapper/keybinding-backEnd-proxy.h"
#include "key-map.h"
#include "shortcut-item.h"
#include "thread-object.h"
#include "ui_shortcut.h"

#define APPLICATION_DIR "/usr/share/applications"
#define TIMEOUT 2000

Shortcut::Shortcut(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Shortcut)
{
    ui->setupUi(this);
    m_keyMap = new KeyMap;
    m_keybindingInterface = new KeybindingBackEndProxy(KEYBINDING_DBUS_NAME,
                                                       KEYBINDING_OBJECT_PATH,
                                                       QDBusConnection::sessionBus());
    initUI();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this] {
                search();
                m_timer->stop();
            });
}

Shortcut::~Shortcut()
{
    delete ui;
    m_thread->quit();
    m_thread->wait();
    if (m_keyMap)
        delete m_keyMap;
    if (m_keybindingInterface)
        delete m_keybindingInterface;
    if (m_timer)
        delete m_timer;
    if (m_shortcutItem.size() > 0)
    {
        foreach (ShortcutItem *item, m_shortcutItem)
        {
            delete item;
            item = nullptr;
        }
    }
    if (m_shortcuts.size() > 0)
    {
        foreach (ShortcutInfo *shortcut, m_shortcuts)
        {
            delete shortcut;
            shortcut = nullptr;
        }
    }

    clearFilterItems();
}

QSize Shortcut::sizeHint() const
{
    return QSize(650, 730);
}

void Shortcut::initUI()
{
    ui->lineEdit_search->setPlaceholderText(tr("Please enter a search keyword..."));

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_shortcut_add, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_page_add, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);

    ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
    ui->stackedWidget_search->setCurrentWidget(ui->page_shortcut_list);

    QList<QLineEdit *> lineEdits = ui->page_add->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, lineEdits)
    {
        lineEdit->setPlaceholderText(tr("Required"));
    }

    QHBoxLayout *hLayoutCustomApp = new QHBoxLayout(ui->lineEdit_custom_app);
    m_btnCustomApp = new QToolButton;
    m_btnCustomApp->setObjectName("btn_custom_app");
    m_btnCustomApp->setText("Add");
    m_btnCustomApp->setFixedSize(56, 30);
    m_btnCustomApp->setCursor(Qt::PointingHandCursor);
    hLayoutCustomApp->addStretch();
    hLayoutCustomApp->addWidget(m_btnCustomApp);
    ui->lineEdit_custom_app->setTextMargins(0, 0, m_btnCustomApp->width(), 0);
    connect(m_btnCustomApp, &QToolButton::clicked, this, &Shortcut::openFileSys);

    QHBoxLayout *hLayoutModifyApp = new QHBoxLayout(ui->lineEdit_modify_app);
    m_btnModifyApp = new QToolButton;
    m_btnModifyApp->setObjectName("btn_modify_app");
    m_btnModifyApp->setText("Add");
    m_btnModifyApp->setFixedSize(56, 30);
    m_btnModifyApp->setCursor(Qt::PointingHandCursor);
    hLayoutModifyApp->addStretch();
    hLayoutModifyApp->addWidget(m_btnModifyApp);
    ui->lineEdit_modify_app->setTextMargins(0, 0, m_btnModifyApp->width(), 0);
    connect(m_btnModifyApp, &QToolButton::clicked, this, &Shortcut::openFileSys);

    m_lECustomKey = new CustomLineEdit;
    m_lECustomKey->setPlaceholderText(tr("Please press the new shortcut key"));
    m_lECustomKey->installEventFilter(this);
    m_lECustomKey->setReadOnly(true);
    ui->vlayout_custom_key->addWidget(m_lECustomKey);
    connect(m_lECustomKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

    m_lEModifyKey = new CustomLineEdit;
    m_lEModifyKey->setPlaceholderText(tr("Please press the new shortcut key"));
    m_lEModifyKey->installEventFilter(this);
    m_lEModifyKey->setReadOnly(true);
    ui->vlayout_modify_key->addWidget(m_lEModifyKey);
    connect(m_lEModifyKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

    connect(ui->btn_shortcut_add, &QPushButton::clicked, this, &Shortcut::handleAddNewShortcut);

    connect(ui->btn_edit, &QToolButton::clicked,
            [this] {
                m_isEditMode = !m_isEditMode;
                if (m_isEditMode)
                    ui->btn_edit->setText(tr("Finished"));
                else
                    ui->btn_edit->setText(tr("Edit"));

                foreach (ShortcutItem *item, m_shortcutItem)
                    item->setEditMode(m_isEditMode);
            });

    connect(ui->btn_cancel, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
            });

    connect(ui->btn_return, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
            });

    connect(ui->lineEdit_search, &QLineEdit::textChanged,
            [this](QString text) {
                if (!text.isEmpty())
                    m_timer->start();
                else
                {
                    m_timer->stop();
                    ui->stackedWidget_search->setCurrentWidget(ui->page_shortcut_list);
                }
            });

    getAllShortcuts();
}

void Shortcut::getAllShortcuts()
{
    m_thread = new QThread;
    m_threadObject = new ThreadObject;

    m_threadObject->moveToThread(m_thread);

    connect(m_threadObject, SIGNAL(getShortcutInfo(QList<ShortcutInfo *>)), this,
            SLOT(handleShortcutInfo(QList<ShortcutInfo *>)));
    //终止线程deleteLater
    connect(m_thread, SIGNAL(finished()), m_threadObject, SLOT(deleteLater()));
    connect(m_thread, SIGNAL(started()), m_threadObject, SLOT(loadShortcutInfo()));

    m_thread->start();
}

ShortcutItem *Shortcut::createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo, int type)
{
    ShortcutItem *item = new ShortcutItem(type, shortcutInfo);
    parent->addWidget(item);

    connect(item, &ShortcutItem::sigClicked, this, &Shortcut::handleEditShortcut);
    connect(item, &ShortcutItem::sigDelete, this, &Shortcut::handleDeleteShortcut);

    return item;
}

bool Shortcut::isConflict(QString &originName, QString newKeyCombination)
{
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (!QString::compare(shortcut->keyCombination, newKeyCombination, Qt::CaseInsensitive))
        {
            originName = shortcut->name;
            return true;
        }
    }
    return false;
}

bool Shortcut::isValidKeycode(QList<int> keycodes)
{
    int count = 0;
    foreach (int keycode, keycodes)
    {
        if (keycode == Qt::Key_Shift || keycode == Qt::Key_Control || keycode == Qt::Key_Alt)
            count++;
    }
    if (count == keycodes.size())
        return false;
    return true;
}

QString Shortcut::convertToString(QList<int> keyCode)
{
    QStringList keyStr;
    QString str;
    foreach (int keycode, keyCode)
    {
        str = m_keyMap->keycodeToString(keycode);
        KLOG_INFO() << str.toLower();
        if (keycode >= 0x30 && keycode <= 0x39)  //数字转化
        {
            keyStr.append(str.split("Key_").at(1));
        }
        else if (keyCode.size() > 1 && SpecialKeyMap.contains(str.toLower()))  //特殊字符转化
        {
            keyStr.append(SpecialKeyMap.value(str.toLower()));
        }
        else
            keyStr.append(m_keyMap->keycodeToString(keycode));
    }
    return keyStr.join("+");  //用于显示
}

QString Shortcut::convertToBackendStr(QString keyStr)
{
    QStringList tmp = keyStr.split("+");
    for (int i = 0; i < tmp.size(); i++)
    {
        if (!tmp.at(i).compare("Alt", Qt::CaseInsensitive) ||
            !tmp.at(i).compare("Shift", Qt::CaseInsensitive) ||
            !tmp.at(i).compare("Ctrl", Qt::CaseInsensitive))
        {
            QString str = "<" + tmp.at(i) + ">";
            tmp.replace(i, str);
        }
    }
    return tmp.join("");
}

bool Shortcut::getExecFromDesktop(QString fileName, QString &exec)
{
    QSettings settings(fileName, QSettings::IniFormat);
    QString str = settings.value("Desktop Entry/Exec").toString();
    if (str.isNull())
        return false;

    exec = str;
    return true;
}

void Shortcut::openFileSys()
{
    QToolButton *senderbtn = qobject_cast<QToolButton *>(sender());
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(senderbtn->parent());

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    APPLICATION_DIR);
    if (fileName.isNull())
        return;

    QString exec = fileName;
    if (fileName.endsWith(".desktop"))
    {
        QString tmp;
        if (!getExecFromDesktop(fileName, tmp))
        {
            KLOG_INFO() << "cant't get Exec key from " << fileName;
            return;
        }
        exec = tmp;
    }

    lineEdit->setText(exec);
}

void Shortcut::search()
{
    KLOG_INFO() << "search.....";
    clearFilterItems();
    QString text = ui->lineEdit_search->text();
    foreach (ShortcutItem *item, m_shortcutItem)
    {
        if (item->getName().contains(text, Qt::CaseInsensitive))
        {
            ui->stackedWidget_search->setCurrentWidget(ui->page_filter_list);

            //            ShortcutItem *filterItem = new ShortcutItem(item->getType(), item->getShortcut());
            ShortcutItem *filterItem = createShortcutItem(ui->vlayout_filter, item->getShortcut(), item->getType());
            m_filterItem.append(filterItem);
        }
    }
}

//custom
void Shortcut::updateShorcut(QString uid, QString name, QString action, QString keyCombination)
{
    //更新快捷键列表
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (shortcut->uid == uid)
        {
            //update
            shortcut->name = name;
            shortcut->action = action;
            shortcut->keyCombination = keyCombination;
            break;
        }
    }
    //更新快捷键项
    foreach (ShortcutItem *item, m_shortcutItem)
    {
        if (item->getUid() == uid)
        {
            item->setname(name);
            item->setKeyBinding(keyCombination);
            item->setAction(action);
        }
    }
    //更新过滤后的快捷键项
    foreach (ShortcutItem *item, m_filterItem)
    {
        if (item->getUid() == uid)
        {
            item->setname(name);
            item->setKeyBinding(keyCombination);
            item->setAction(action);
        }
    }
}

//system
void Shortcut::updateShorcut(QString uid, QString keyCombination)
{
    //更新快捷键列表
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (shortcut->uid == uid)
        {
            //update
            shortcut->keyCombination = keyCombination;
            break;
        }
    }
    //更新快捷键项
    foreach (ShortcutItem *item, m_shortcutItem)
    {
        if (item->getUid() == uid)
        {
            item->setKeyBinding(keyCombination);
            break;
        }
    }
    //更新过滤后的快捷键项
    foreach (ShortcutItem *item, m_filterItem)
    {
        if (item->getUid() == uid)
        {
            item->setKeyBinding(keyCombination);
        }
    }
}

void Shortcut::clearFilterItems()
{
    foreach (ShortcutItem *item, m_filterItem)
    {
        if (item)
        {
            m_filterItem.removeOne(item);
            delete item;
            item = nullptr;
            update();
        }
    }
}

void Shortcut::connectDbusSignal()
{
    connect(m_keybindingInterface, &KeybindingBackEndProxy::Added, this, &Shortcut::handleDbusChanges);
}

void Shortcut::addShortcut()
{
}

void Shortcut::handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList)
{
    m_shortcuts = shortcutInfoList;

    ShortcutItem *item;
    foreach (ShortcutInfo *shortcutInfo, shortcutInfoList)
    {
        if (shortcutInfo->type == SHORTCUT_TYPE_SYSTEM)
        {
            if (shortcutInfo->kind == SHORTCUT_KIND_SOUND)
                item = createShortcutItem(ui->vlayout_sound, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_WINDOW_MANAGE)
                item = createShortcutItem(ui->vlayout_window_manage, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_SYSTEM)
                item = createShortcutItem(ui->vlayout_system, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_ACCESSIBILITY)
                item = createShortcutItem(ui->vlayout_accessibility, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_DESKTOP)
                item = createShortcutItem(ui->vlayout_desktop, shortcutInfo, shortcutInfo->type);
        }
        else
        {
            item = createShortcutItem(ui->vlayout_custom, shortcutInfo, shortcutInfo->type);
            m_customShortcutCount++;
        }
        m_shortcutItem.append(item);
    }

    if (m_customShortcutCount == 0)
    {
        ui->widget_custom->hide();
    }
}

void Shortcut::handleEditShortcut(int type, QString uid, QString name, QString keyCombination, QString action)
{
    ShortcutItem *item = qobject_cast<ShortcutItem *>(sender());
    ui->stackedWidget->setCurrentWidget(ui->page_modify);
    m_lEModifyKey->clear();
    ui->lineEdit_modify_name->setText(name);

    if (type == SHORTCUT_TYPE_SYSTEM)
    {
        ui->widget_modify_app->hide();
        ui->lineEdit_modify_name->setDisabled(true);
    }
    else
    {
        ui->widget_modify_app->show();
        ui->lineEdit_modify_app->setText(action);
        ui->lineEdit_modify_name->setDisabled(false);
    }
    connect(ui->btn_save, &QPushButton::clicked,
            [=] {
                QString newKeyCombination = convertToBackendStr(m_lEModifyKey->text());
                if (type == SHORTCUT_TYPE_SYSTEM)
                {
                    QDBusPendingReply<> reply = m_keybindingInterface->ModifySystemShortcut(uid, newKeyCombination);
                    reply.waitForFinished();
                    if (reply.isError() || !reply.isValid())
                    {
                        KLOG_DEBUG() << "Call ModifySystemShortcut method failed "
                                     << " Error: " << reply.error().message();
                        return;
                    }
                    else
                    {
                        ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
                        updateShorcut(uid, newKeyCombination);
                    }
                }
                else
                {
                    QString newName = ui->lineEdit_modify_name->text();
                    QString newAction = ui->lineEdit_modify_app->text();
                    QDBusPendingReply<> reply = m_keybindingInterface->ModifyCustomShortcut(uid, newName, newAction, newKeyCombination);
                    reply.waitForFinished();
                    if (reply.isError() || !reply.isValid())
                    {
                        KLOG_DEBUG() << "Call ModifyCustomShortcut method failed "
                                     << " Error: " << reply.error().message();
                        return;
                    }
                    else
                    {
                        ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
                        updateShorcut(uid, newName, newAction, newKeyCombination);
                    }
                }
            });
}

void Shortcut::handleDbusChanges(QString result)
{
    QString uid;
    QString kind;
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_DEBUG() << " please check the string " << result.toLocal8Bit().data();
        return;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();
        QJsonValue val;
        val = obj.value(KEYBINDING_SHORTCUT_JK_UID);
        uid = val.toString();
        val = obj.value(KEYBINDING_SHORTCUT_JK_KIND);
        kind = val.toString();
    }
}

void Shortcut::handleDeleteShortcut(QString uid)
{
    KLOG_INFO() << "deleteShortcut";
    ShortcutItem *item = qobject_cast<ShortcutItem *>(sender());
    QDBusPendingReply<> reply = m_keybindingInterface->DeleteCustomShortcut(uid);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call DeleteCustomShortcut method failed "
                     << " Error: " << reply.error().message();
        return;
    }
    else
    {
        m_shortcutItem.removeOne(item);
        ShortcutInfo *shortcut = item->getShortcut();
        m_shortcuts.removeOne(shortcut);
        item->deleteLater();
        m_customShortcutCount--;
        if (m_customShortcutCount == 0)
        {
            ui->widget_custom->hide();
            ui->btn_edit->setText(tr("Edit"));
        }
    }
}

void Shortcut::handleInputKeycode(QList<int> keycodes)
{
    CustomLineEdit *senderLineEdit = qobject_cast<CustomLineEdit *>(sender());
    //转化成字符串列表
    QString keyStr = convertToString(keycodes);  //用于显示

    //判断单个key是否在ignoreKey中
    if (keycodes.size() == 1)
    {
        KLOG_INFO() << keyStr;
        if (ignoreKeys.contains(keyStr, Qt::CaseInsensitive) ||
            keyStr.contains(QRegExp("[A-Z]")) ||
            keyStr.contains(QRegExp("[0-9]")))
        {
            KiranMessageBox::message(nullptr,
                                     tr("Failed"),
                                     QString(tr("Cannot use shortcut \"%1\", Because you cannot enter with this key."
                                                "Please try again using Ctrl, alt, or shift at the same time."))
                                         .arg(keyStr),
                                     KiranMessageBox::Ok);
            return;
        }
    }
    //判断快捷键输入是否合法（排除都是修饰键的情况）
    if (!isValidKeycode(keycodes))
        return;

    QString keyCombination = convertToBackendStr(keyStr);
    //判断是否重复
    QString originName;
    if (isConflict(originName, keyCombination))
    {
        KiranMessageBox::message(nullptr,
                                 QString(tr("Failed")),
                                 QString(tr("Shortcut keys %1 are already used in %2,If you reassign the shortcut keys, %2 Shortcut keys for will be disabled.")).arg(keyStr).arg(originName),
                                 KiranMessageBox::Cancel | KiranMessageBox::Ok);
        KLOG_INFO() << "isConflict";
        m_lECustomKey->clear();
        return;
    }
    KLOG_INFO() << "ok: " << keyStr;

    //显示在输入框中
    senderLineEdit->setText(keyStr);
    senderLineEdit->clearFocus();
}

void Shortcut::handleAddNewShortcut()
{
    ui->stackedWidget->setCurrentWidget(ui->page_add);
    ui->lineEdit_custom_app->clear();
    ui->lineEdit_custom_name->clear();
    m_lECustomKey->clear();

    connect(ui->btn_page_add, &QPushButton::clicked,
            [=] {
                KLOG_INFO() << "handleAddNewShortcut";
                QString newName = ui->lineEdit_custom_name->text();
                QString newAction = ui->lineEdit_custom_app->text();
                QString newKey = m_lECustomKey->text();
                if (newName.isEmpty() || newAction.isEmpty() || newKey.isEmpty())
                {
                    KiranMessageBox::message(nullptr,
                                             tr("Warning"),
                                             tr("Please complete the shortcut information!"),
                                             KiranMessageBox::Ok);
                }

                //dbus ->AddCustomShortcut
                QString keyCombination = convertToBackendStr(newKey);
                QDBusPendingReply<QString> reply = m_keybindingInterface->AddCustomShortcut(newName, newAction, keyCombination);
                reply.waitForFinished();
                if (reply.isError() || !reply.isValid())
                {
                    KLOG_DEBUG() << "Call AddCustomShortcut method failed "
                                 << " Error: " << reply.error().message();
                    return;
                }
                else
                {
                    QString uid = reply.argumentAt(0).toString();
                    ShortcutInfo *shortcut = new ShortcutInfo;
                    shortcut->name = newName;
                    shortcut->action = newAction;
                    shortcut->keyCombination = keyCombination;
                    shortcut->type = SHORTCUT_TYPE_CUSTOM;
                    shortcut->uid = uid;
                    m_shortcuts.append(shortcut);

                    ui->widget_custom->show();
                    ShortcutItem *item = createShortcutItem(ui->vlayout_custom, shortcut, SHORTCUT_TYPE_CUSTOM);
                    m_shortcutItem.append(item);
                    ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
                    m_customShortcutCount++;
                }
            });
}

//解决输入Ctrl+v会显示剪切板中的内容
bool Shortcut::eventFilter(QObject *target, QEvent *event)
{
    if (target == m_lECustomKey || target == m_lEModifyKey)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Paste))
            {
                KLOG_INFO() << "Ctrl + V";
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
