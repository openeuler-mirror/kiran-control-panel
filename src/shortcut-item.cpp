#include "shortcut-item.h"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "ui_shortcut-item.h"
ShortcutItem::ShortcutItem(int type, ShortcutInfo *shortcutInfo, QWidget *parent) : QWidget(parent),
                                                                                    ui(new Ui::ShortcutItem)

{
    ui->setupUi(this);
    m_type = type;
    m_name = shortcutInfo->name;
    m_keyCombination = shortcutInfo->keyCombination;
    m_uid = shortcutInfo->uid;
    m_action = shortcutInfo->action;

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

    ui->label_name->setText(m_name);

    ui->label_keybination->setText(m_keyCombination);

    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                sigDelete(m_uid);
            });
}

void ShortcutItem::setname(QString name)
{
    m_name = name;
    ui->label_name->setText(m_name);
}

void ShortcutItem::setKeyBinding(QString keyCombination)
{
    m_keyCombination = keyCombination;
    ui->label_keybination->setText(m_keyCombination);
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
        emit sigClicked(m_type, m_uid, m_name, m_keyCombination, m_action);
    }
    QWidget::mousePressEvent(event);
}

QString ShortcutItem::getName()
{
    return m_name;
}

QString ShortcutItem::getUid()
{
    return m_uid;
}
