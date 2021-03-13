//
// Created by lxh on 2021/3/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_biometric-item.h" resolved

#include "biometric-item.h"
#include "ui_biometric-item.h"
#include <QStyleOption>
#include <QPainter>

BiometricItem::BiometricItem(const QString &text,
                             const QString &biometricID,
                             BiometricItemType type,
                             QWidget *parent)
                             :QWidget(parent),
                             ui(new Ui::BiometricItem),
                             m_itemType(type),
                             m_biometricItemDataID(biometricID){
    ui->setupUi(this);
    initUI(text,biometricID,type);
}

BiometricItem::~BiometricItem() {
    delete ui;
}

void BiometricItem::initUI(const QString& text,const QString& biometricID,BiometricItemType type) {
    setFixedHeight(40);
    ui->label_text->setText(text);

    switch (type) {
        case BIOMETRIC_ITEM_ADD:
            ui->btn_add->setVisible(true);
            ui->btn_delete->setVisible(false);
            break;
        case BIOMETRIC_ITEM_NORMAL:
            ui->btn_delete->setVisible(true);
            ui->btn_add->setVisible(false);
            break;
    }

    connect(ui->btn_add,&QPushButton::clicked,[this](){
        Q_EMIT sigAddBiometricItem();
    });

    connect(ui->btn_delete,&QPushButton::clicked,[=](){
        Q_EMIT sigDeleteBiometricItem(text);
    });
}

void BiometricItem::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BiometricItem::setItemAddEnabled(bool enabled) {
    ui->btn_add->setEnabled(enabled);
}

BiometricItem::BiometricItemType BiometricItem::getItemType() {
    return m_itemType;
}

QString BiometricItem::getBiometricItemName() {
    return ui->label_text->text();
}

QString BiometricItem::getBiometricItemDataID() {
    return m_biometricItemDataID;
}
