 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
#ifndef LINEEDITWITHCHECKICON_H
#define LINEEDITWITHCHECKICON_H

#include <kiran-icon-line-edit.h>
#include <QWidget>

class LineEditWithCheckIcon : public KiranIconLineEdit
{
    Q_OBJECT

public:
    enum VerificationStatusEnum
    {
        VERIFICATION_PASSED,
        VERIFICATION_ERROR,
        VERIFICATION_NONE
    };
    Q_ENUM(VerificationStatusEnum)

public:
    explicit LineEditWithCheckIcon(QWidget *parent = nullptr);
    ~LineEditWithCheckIcon();

    void resetVerificationStatus();
    void setVerificationStatus(bool isPassed);

private:
    void initUI();
    ///@biref 设置普通字符间距
    void setNormalLetterSpacing();
    ///@biref 设置密码字符间距
    void setPasswdLetterSpacing();
    ///@biref 设置输入密码样式
    void setShowPasswordModeStyle(bool showPasswordModeStyle);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;

private:
    VerificationStatusEnum m_verificationStatus;
    bool m_showPasswordModeStyle;
};

#endif  // LINEEDITWITHCHECKICON_H
