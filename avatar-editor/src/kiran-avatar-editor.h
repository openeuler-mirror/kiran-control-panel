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
