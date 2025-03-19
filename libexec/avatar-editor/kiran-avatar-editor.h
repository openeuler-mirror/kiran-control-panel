/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRAN_AVATAR_EDITOR_KIRAN_AVATAR_EDITOR_H
#define KIRAN_AVATAR_EDITOR_KIRAN_AVATAR_EDITOR_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>

class ImagePreviewWidget;
class QLayout;
class QPushButton;

class KiranAvatarEditor : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit KiranAvatarEditor(const QString &srcImagePath, const QString &dstImagePath, QWidget *parent = nullptr);
    ~KiranAvatarEditor();

private:
    void initUI(const QString &srcImagePath,
                const QString &dstImagePath);

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    QLayout *m_layout = nullptr;
    ImagePreviewWidget *m_imagePreview = nullptr;
    QPushButton *m_btnReopen = nullptr;
    QPushButton *m_btnConfrim = nullptr;
    QPushButton *m_btnCancel = nullptr;
    QWidget *m_buttonWidget = nullptr;
};

#endif  //KIRAN_AVATAR_EDITOR_KIRAN_AVATAR_EDITOR_H
