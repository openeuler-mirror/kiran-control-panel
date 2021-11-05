/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keybinding is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "shortcut.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/keybinding-i.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <QClipboard>
#include <QFileDialog>
#include <QKeyEvent>
#include "custom-line-edit.h"
#include "key-map.h"
#include "keybinding_backEnd_proxy.h"
#include "shortcut-item.h"
#include "thread-object.h"
#include "ui_shortcut.h"

#define APPLICATION_DIR "/usr/share/applications"
#define TIMEOUT 200

Shortcut::Shortcut(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Shortcut)
{
    ui->setupUi(this);
    m_keyMap = new KeyMap;
    m_keybindingInterface = new KeybindingBackEndProxy(KEYBINDING_DBUS_NAME,
                                                       KEYBINDING_OBJECT_PATH,
                                                       QDBusConnection::sessionBus());

    connect(m_keybindingInterface, &KeybindingBackEndProxy::Added, this, &Shortcut::addShortcut);
    connect(m_keybindingInterface, &KeybindingBackEndProxy::Deleted, this, &Shortcut::deleteShortcut);
    connect(m_keybindingInterface, &KeybindingBackEndProxy::Changed, this, &Shortcut::editShortcut);

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
    return QSize(700, 730);
}

void Shortcut::initUI()
{
    ui->lineEdit_search->setPlaceholderText(tr("Please enter a search keyword..."));

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_shortcut_add, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_page_add, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);
    ui->btn_reset->setDisabled(true);

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
    m_btnCustomApp->setText(tr("Add"));
    m_btnCustomApp->setFixedSize(56, 30);
    m_btnCustomApp->setCursor(Qt::PointingHandCursor);
    hLayoutCustomApp->addStretch();
    hLayoutCustomApp->addWidget(m_btnCustomApp);
    ui->lineEdit_custom_app->setTextMargins(0, 0, m_btnCustomApp->width(), 0);
    connect(m_btnCustomApp, &QToolButton::clicked, this, &Shortcut::openFileSys);

    QHBoxLayout *hLayoutModifyApp = new QHBoxLayout(ui->lineEdit_modify_app);
    m_btnModifyApp = new QToolButton;
    m_btnModifyApp->setObjectName("btn_modify_app");
    m_btnModifyApp->setText(tr("Add"));
    m_btnModifyApp->setFixedSize(56, 30);
    m_btnModifyApp->setCursor(Qt::PointingHandCursor);
    hLayoutModifyApp->addStretch();
    hLayoutModifyApp->addWidget(m_btnModifyApp);
    ui->lineEdit_modify_app->setTextMargins(0, 0, m_btnModifyApp->width(), 0);
    connect(m_btnModifyApp, &QToolButton::clicked, this, &Shortcut::openFileSys);

    m_lECustomKey = new CustomLineEdit;
    m_lECustomKey->installEventFilter(this);
    ui->vlayout_custom_key->addWidget(m_lECustomKey);
    connect(m_lECustomKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

    m_lEModifyKey = new CustomLineEdit;
    m_lEModifyKey->installEventFilter(this);
    ui->vlayout_modify_key->addWidget(m_lEModifyKey);
    connect(m_lEModifyKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

    getAllShortcuts();

    connect(ui->btn_shortcut_add, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_add);
                ui->lineEdit_custom_app->clear();
                ui->lineEdit_custom_name->clear();
                m_lECustomKey->clear();
            });

    connect(ui->btn_save, &QPushButton::clicked, this, &Shortcut::onSave);
    connect(ui->btn_page_add, &QPushButton::clicked, this, &Shortcut::onAdd);

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
                    m_timer->start(TIMEOUT);
                else
                {
                    m_timer->stop();
                    ui->stackedWidget_search->setCurrentWidget(ui->page_shortcut_list);
                }
            });
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

ShortcutInfo *Shortcut::getShortcut(QString uid, QString kind)
{
    ShortcutInfo *shortcut = new ShortcutInfo;
    QDBusPendingReply<QString> reply;
    int type;
    if (kind == SHORTCUT_KIND_CUSTOM)
    {
        reply = m_keybindingInterface->GetCustomShortcut(uid);
        type = SHORTCUT_TYPE_CUSTOM;
    }
    else
    {
        reply = m_keybindingInterface->GetSystemShortcut(uid);
        type = SHORTCUT_TYPE_SYSTEM;
    }

    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_ERROR() << "Call GetShortcut method failed "
                     << " Error: " << reply.error().message();
        return nullptr;
    }
    else
    {
        QMap<QString, QString> info;
        getJsonValue(reply.argumentAt(0).toString(), info);
        shortcut->uid = info.value(KEYBINDING_SHORTCUT_JK_UID);
        shortcut->action = info.value(KEYBINDING_SHORTCUT_JK_ACTION);
        shortcut->name = info.value(KEYBINDING_SHORTCUT_JK_NAME);
        shortcut->keyCombination = info.value(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION);
        shortcut->kind = info.value(KEYBINDING_SHORTCUT_JK_KIND);
        shortcut->type = type;
    }
    return shortcut;
}

ShortcutItem *Shortcut::createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo, int type)
{
    ShortcutItem *item = new ShortcutItem(type, shortcutInfo);
    parent->addWidget(item);

    connect(item, &ShortcutItem::sigClicked, this, &Shortcut::onEditShortcut);
    connect(item, &ShortcutItem::sigDelete, this, &Shortcut::onDeleteShortcut);

    return item;
}

bool Shortcut::isConflict(QString &originName, QString newKeyCombination)
{
    KLOG_INFO() << "isConflict:" << newKeyCombination;
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (!QString::compare(shortcut->keyCombination, newKeyCombination, Qt::CaseInsensitive))
        {
            KLOG_INFO() << "yes";
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
        else if (keycode >= 0x01000070 && keycode <= 0x01000113)
        {
            keyStr.append(str.split("_").join(" "));
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
        //modifier
        if (!tmp.at(i).compare("Alt", Qt::CaseInsensitive) ||
            !tmp.at(i).compare("Shift", Qt::CaseInsensitive) ||
            !tmp.at(i).compare("Ctrl", Qt::CaseInsensitive))
        {
            QString str = "<" + tmp.at(i) + ">";
            KLOG_INFO() << "modifier:" << str;
            tmp.replace(i, str);
        }
        //media key
        else if (tmp.at(i).contains(" "))
        {
            KLOG_INFO() << "media key:" << tmp.at(i);
            QString str = QString("XF86%1").arg(tmp.at(i).split(" ").join(""));
            tmp.replace(i, str);
        }
        //special key
        else if (!tmp.at(i).contains(QRegExp("[A-Z]")) &&
                 !tmp.at(i).contains(QRegExp("[a-z]")) &&
                 !tmp.at(i).contains(QRegExp("[0-9]")))
        {
            KLOG_INFO() << "special key:" << tmp.at(i);
            tmp.replace(i, SpecialKeyMap.key(tmp.at(i)));
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
            ShortcutItem *filterItem = createShortcutItem(ui->vlayout_filter, item->getShortcut(), item->getType());
            m_filterItem.append(filterItem);
        }
    }
    ui->stackedWidget_search->setCurrentWidget(ui->page_filter_list);
}

//custom
void Shortcut::updateShorcut(ShortcutInfo *newShortcut)
{
    //更新快捷键列表
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (shortcut->uid == newShortcut->uid)
        {
            //update
            shortcut->name = newShortcut->name;
            shortcut->action = newShortcut->action;
            shortcut->keyCombination = newShortcut->keyCombination;
            break;
        }
    }
    //更新快捷键项
    foreach (ShortcutItem *item, m_shortcutItem)
    {
        if (item->getUid() == newShortcut->uid)
        {
            item->setname(newShortcut->name);
            item->setKeyBinding(newShortcut->keyCombination);
            item->setAction(newShortcut->action);
            break;
        }
    }
    //更新过滤后的快捷键项
    foreach (ShortcutItem *item, m_filterItem)
    {
        if (item->getUid() == newShortcut->uid)
        {
            item->setname(newShortcut->name);
            item->setKeyBinding(newShortcut->keyCombination);
            item->setAction(newShortcut->action);
            break;
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

void Shortcut::insertShortcut(ShortcutInfo *shortcutInfo)
{
    ShortcutItem *item;
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
    m_shortcuts.append(shortcutInfo);
    if (m_customShortcutCount == 0)
        ui->widget_custom->hide();
    else
        ui->widget_custom->show();
}

void Shortcut::addShortcut(QString result)
{
    KLOG_INFO() << "get Added signal from dbus";
    QMap<QString, QString> info;
    getJsonValue(result, info);  //uid,kind

    QString uid = info.value(KEYBINDING_SHORTCUT_JK_UID);
    QString kind = info.value(KEYBINDING_SHORTCUT_JK_KIND);
    KLOG_INFO() << uid << "," << kind;

    ShortcutInfo *shortcut = getShortcut(uid, kind);
    insertShortcut(shortcut);
}

void Shortcut::deleteShortcut(QString result)
{
    KLOG_INFO() << "get Delete signal from dbus";
    QMap<QString, QString> info;
    getJsonValue(result, info);  //uid,kind

    QString uid = info.value(KEYBINDING_SHORTCUT_JK_UID);
    QString kind = info.value(KEYBINDING_SHORTCUT_JK_KIND);
    KLOG_INFO() << uid << "," << kind;

    foreach (ShortcutItem *item, m_shortcutItem)
    {
        if (item->getUid() == uid)
        {
            m_shortcutItem.removeOne(item);
            ShortcutInfo *shortcut = item->getShortcut();
            m_shortcuts.removeOne(shortcut);
            if (shortcut->type == SHORTCUT_TYPE_CUSTOM)
            {
                m_customShortcutCount--;
                if (m_customShortcutCount == 0)
                {
                    ui->widget_custom->hide();
                    ui->btn_edit->setText(tr("Edit"));
                }
            }
            delete item;
            item = nullptr;
            break;
        }
    }
}

void Shortcut::editShortcut(QString result)
{
    KLOG_INFO() << "get Change signal from dbus";
    QMap<QString, QString> info;
    getJsonValue(result, info);  //uid,kind

    QString uid = info.value(KEYBINDING_SHORTCUT_JK_UID);
    QString kind = info.value(KEYBINDING_SHORTCUT_JK_KIND);
    KLOG_INFO() << uid << "," << kind;

    ShortcutInfo *shortcut = getShortcut(uid, kind);
    updateShorcut(shortcut);
}

int Shortcut::getJsonValue(QString result, QMap<QString, QString> &info)
{
    QString uid;
    QString kind;
    QString action;
    QString keyCombination;
    QString name;
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_ERROR() << " please check the string " << result.toLocal8Bit().data();
        return 0;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();
        QJsonValue val;
        if (obj.contains(KEYBINDING_SHORTCUT_JK_UID))
        {
            val = obj.value(KEYBINDING_SHORTCUT_JK_UID);
            uid = val.toString();
            info.insert(KEYBINDING_SHORTCUT_JK_UID, uid);
        }
        if (obj.contains(KEYBINDING_SHORTCUT_JK_KIND))
        {
            val = obj.value(KEYBINDING_SHORTCUT_JK_KIND);
            kind = val.toString();
            info.insert(KEYBINDING_SHORTCUT_JK_KIND, kind);
        }
        if (obj.contains(KEYBINDING_SHORTCUT_JK_NAME))
        {
            val = obj.value(KEYBINDING_SHORTCUT_JK_NAME);
            name = val.toString();
            info.insert(KEYBINDING_SHORTCUT_JK_NAME, name);
        }
        if (obj.contains(KEYBINDING_SHORTCUT_JK_ACTION))
        {
            val = obj.value(KEYBINDING_SHORTCUT_JK_ACTION);
            action = val.toString();
            info.insert(KEYBINDING_SHORTCUT_JK_ACTION, action);
        }
        if (obj.contains(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION))
        {
            val = obj.value(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION);
            keyCombination = val.toString();
            info.insert(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION, keyCombination);
        }
    }
    return info.size();
}

void Shortcut::handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList)
{
    foreach (ShortcutInfo *shortcutInfo, shortcutInfoList)
    {
        insertShortcut(shortcutInfo);
    }
}

void Shortcut::onEditShortcut(int type, QString uid, QString name, QString keyCombination, QString action)
{
    Q_UNUSED(keyCombination)
    ui->stackedWidget->setCurrentWidget(ui->page_modify);
    m_lEModifyKey->clear();
    m_editUid = uid;

    ui->lineEdit_modify_name->setText(name);
    ui->lineEdit_modify_app->setText(action);

    if (type == SHORTCUT_TYPE_SYSTEM)
    {
        ui->widget_modify_app->hide();
        ui->lineEdit_modify_name->setDisabled(true);
    }
    else
    {
        ui->widget_modify_app->show();
        ui->lineEdit_modify_name->setDisabled(false);
    }
}

void Shortcut::onDeleteShortcut(QString uid)
{
    QDBusPendingReply<> reply = m_keybindingInterface->DeleteCustomShortcut(uid);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_ERROR() << "Call DeleteCustomShortcut method failed "
                     << " Error: " << reply.error().message();
        return;
    }
}

void Shortcut::onSave()
{
    int type = ui->lineEdit_modify_app->isVisible() ? SHORTCUT_TYPE_CUSTOM : SHORTCUT_TYPE_SYSTEM;
    if (ui->lineEdit_modify_name->text().isEmpty() ||
        (ui->lineEdit_modify_app->text().isEmpty() && type == SHORTCUT_TYPE_CUSTOM) ||
        m_lEModifyKey->text().isEmpty())
    {
        KiranMessageBox::message(nullptr,
                                 tr("Warning"),
                                 tr("Please complete the shortcut information!"),
                                 KiranMessageBox::Ok);
        return;
    }

    QString newKeyCombination = convertToBackendStr(m_lEModifyKey->text());
    //    //判断是否重复
    //    QString originName;
    //    if (isConflict(originName, newKeyCombination))
    //    {
    //        KiranMessageBox::message(nullptr,
    //                                 QString(tr("Failed")),
    //                                 QString(tr("Shortcut keys %1 are already used in %2,Please try again!")).arg(m_lEModifyKey->text()).arg(originName),
    //                                 KiranMessageBox::Ok);
    //        m_lECustomKey->clear();
    //        return;
    //    }

    if (type == SHORTCUT_TYPE_SYSTEM)
    {
        QDBusPendingReply<> reply = m_keybindingInterface->ModifySystemShortcut(m_editUid, newKeyCombination);
        reply.waitForFinished();
        if (reply.isError() || !reply.isValid())
        {
            KLOG_ERROR() << "Call ModifySystemShortcut method failed "
                         << " Error: " << reply.error().message();
            return;
        }
        else
            ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
    }
    else
    {
        QString newName = ui->lineEdit_modify_name->text();
        QString newAction = ui->lineEdit_modify_app->text();
        QDBusPendingReply<> reply = m_keybindingInterface->ModifyCustomShortcut(m_editUid, newName, newAction, newKeyCombination);
        reply.waitForFinished();
        if (reply.isError() || !reply.isValid())
        {
            KLOG_ERROR() << "Call ModifyCustomShortcut method failed "
                         << " Error: " << reply.error().message();
            return;
        }
        else
            ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
    }
}

void Shortcut::onAdd()
{
    QString newName = ui->lineEdit_custom_name->text();
    QString newAction = ui->lineEdit_custom_app->text();
    QString newKey = m_lECustomKey->text();
    if (newName.isEmpty() || newAction.isEmpty() || newKey.isEmpty())
    {
        KiranMessageBox::message(nullptr,
                                 tr("Warning"),
                                 tr("Please complete the shortcut information!"),
                                 KiranMessageBox::Ok);
        return;
    }

    //dbus ->AddCustomShortcut
    QString keyCombination = convertToBackendStr(newKey);
    QDBusPendingReply<QString> reply = m_keybindingInterface->AddCustomShortcut(newName, newAction, keyCombination);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_ERROR() << "Call AddCustomShortcut method failed "
                     << " Error: " << reply.error().message();
        return;
    }
    else
        ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
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
                                                "Please try again using Ctrl, Alt, or Shift at the same time."))
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
                                 QString(tr("Shortcut keys %1 are already used in %2,Please try again!")).arg(keyStr).arg(originName),
                                 KiranMessageBox::Ok);
        m_lECustomKey->clear();
        return;
    }

    //显示在输入框中
    senderLineEdit->setText(keyStr);
    senderLineEdit->clearFocus();
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
