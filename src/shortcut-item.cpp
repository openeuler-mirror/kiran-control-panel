#include "shortcut-item.h"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "ui_shortcut-item.h"
ShortcutItem::ShortcutItem(int type, ShortcutInfo *shortcutInfo, QWidget *parent) : QWidget(parent),
                                                                                    ui(new Ui::ShortcutItem)

{
    ui->setupUi(this);
    m_shortcutInfo = shortcutInfo;
    m_type = type;
    initUI();
}

ShortcutItem::~ShortcutItem()
{
    delete ui;
}

void ShortcutItem::initUI()
{
    ui->btn_delete->hide();
    ui->btn_delete->setIcon(QIcon(":/images/delete.svg"));

    ui->label_name->setText(m_shortcutInfo->name);

    ui->label_keybination->setText(handleKeyCombination(m_shortcutInfo->keyCombination));

    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                sigDelete(m_shortcutInfo->uid);
            });
}

QString ShortcutItem::handleKeyCombination(QString origStr)
{
    QString keyCombination;
    if (origStr.isEmpty())
    {
        keyCombination = QString(tr("None"));
    }
    else if (origStr.contains("disable", Qt::CaseInsensitive))
    {
        keyCombination = QString(tr("disabled"));
    }
    else
    {
        origStr = origStr.replace("<", "");
        origStr = origStr.replace(">", "-");
        QStringList list = origStr.split("-", QString::SkipEmptyParts);
        //handle speciel key
        for (int i = 0; i < list.size(); i++)
        {
            if (SpecialKeyMap.contains(list.at(i).toLower()))
            {
                list.replace(i, SpecialKeyMap.value(list.at(i).toLower()));
            }
            if (MediaKeyMap.contains(list.at(i).toLower()))
            {
                list.replace(i, MediaKeyMap.value(list.at(i)));
            }
        }

        keyCombination = list.join("+");
    }
    return keyCombination;
}

void ShortcutItem::setname(QString name)
{
    m_shortcutInfo->name = name;
    ui->label_name->setText(name);
}

void ShortcutItem::setKeyBinding(QString keyCombination)
{
    keyCombination = keyCombination.replace("<", "");
    keyCombination = keyCombination.replace(">", "-");
    QStringList list = keyCombination.split("-", QString::SkipEmptyParts);

    m_shortcutInfo->keyCombination = keyCombination;
    ui->label_keybination->setText(list.join("+"));
}

void ShortcutItem::setAction(QString action)
{
    m_shortcutInfo->action = action;
}

void ShortcutItem::setEditMode(bool isEditMode)
{
    if (m_type == SHORTCUT_TYPE_CUSTOM)
        ui->btn_delete->setVisible(isEditMode);
}

void ShortcutItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void ShortcutItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit sigClicked(m_type, m_shortcutInfo->uid, m_shortcutInfo->name, m_shortcutInfo->keyCombination, m_shortcutInfo->action);
    }
    QWidget::mousePressEvent(event);
}

QString ShortcutItem::getName()
{
    return m_shortcutInfo->name;
}

int ShortcutItem::getType()
{
    return m_shortcutInfo->type;
}

QString ShortcutItem::getUid()
{
    return m_shortcutInfo->uid;
}

ShortcutInfo *ShortcutItem::getShortcut()
{
    return m_shortcutInfo;
}
