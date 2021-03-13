/**
 *@file  biometric-item.h
 *@brief 生物识别条目，在认证管理中显示的条目
 *@auth  liuxinhao <liuxinhao@kylinos.com.cn>
 *@copyright (c)  KylinSec. All rights reserved.
 */

#ifndef KIRAN_ACCOUNT_MANAGER_BIOMETRIC_ITEM_H
#define KIRAN_ACCOUNT_MANAGER_BIOMETRIC_ITEM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class BiometricItem; }
QT_END_NAMESPACE

class BiometricItem : public QWidget {
    Q_OBJECT
public:
    enum BiometricItemType{
        BIOMETRIC_ITEM_ADD,
        BIOMETRIC_ITEM_NORMAL
    };
    /// @brief 构造一个生物识别特征的条目
    /// @param text         若是普通条目则为该生物识别的别名，若是添加按钮则为显示的文字
    /// @param biometricID  生物识别ID
    /// @param type         该条的类型
    /// @param parent       控件的父控件
    explicit BiometricItem(const QString &text,
                           const QString &biometricID,
                           BiometricItemType type,
                           QWidget *parent = nullptr);
    ~BiometricItem() override;

    void setItemAddEnabled(bool enabled);
    BiometricItemType getItemType();
    QString getBiometricItemName();
    QString getBiometricItemDataID();

Q_SIGNALS:
    void sigAddBiometricItem();
    void sigDeleteBiometricItem(const QString &biometricID);

private:
    void initUI(const QString& text,const QString& biometricID,BiometricItemType type);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::BiometricItem *ui;
    BiometricItemType m_itemType;
    QString m_biometricItemDataID;
};

#endif //KIRAN_ACCOUNT_MANAGER_BIOMETRIC_ITEM_H
