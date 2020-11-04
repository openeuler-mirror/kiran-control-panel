#ifndef CREATEUSERPAGE_H
#define CREATEUSERPAGE_H

#include <QWidget>

#include "kiran-tips.h"
#include "advance-settings.h"

namespace Ui {
class CreateUserPage;
}

class CreateUserPage : public QWidget
{
    Q_OBJECT

public:
    explicit CreateUserPage(QWidget *parent = nullptr);
    ~CreateUserPage();

    ///@brief 重置界面信息，每次切换到创建用户界面时需重置信息
    void reset();

    ///@brief 设置创建用户界面用户图标，在从用户头像选择界面中退出时设置更新
    void setAvatarIconPath(const QString &iconPath);

signals:
    ///@brief 当用户点击头像时，请求切换到头像选择界面，参数为当前头像的路径
    void sigSetIconForNewUser(QString iconPath);
    ///@brief 请求设置侧边栏当前条目
    void sigRequestSetCurrentUser(const QString& userPath);

private:
    void initUI();

private Q_SLOTS:
    void handlerCreateNewUser();

private:
    Ui::CreateUserPage *ui;
    AdvanceSettingsInfo m_advanceSettingsInfo;
    KiranTips *m_errorTip;

};

#endif // CREATEUSERPAGE_H
