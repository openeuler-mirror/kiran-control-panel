//
// Created by lxh on 2020/10/22.
//

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
    explicit KiranAvatarEditor(const QString &srcImagePath, const QString &dstImagePath, QWidget *parent= nullptr);
    ~KiranAvatarEditor();

private:
    void initUI(const QString &srcImagePath,
                const QString &dstImagePath);

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    QLayout *           m_layout       = nullptr;
    ImagePreviewWidget *m_imagePreview = nullptr;
    QPushButton *       m_btnReopen    = nullptr;
    QPushButton *       m_btnConfrim   = nullptr;
    QPushButton *       m_btnCancel    = nullptr;
    QWidget *           m_buttonWidget = nullptr;
};

#endif  //KIRAN_AVATAR_EDITOR_KIRAN_AVATAR_EDITOR_H
