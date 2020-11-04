//
// Created by lxh on 2020/10/22.
//
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>

#include "kiran-avatar-editor.h"
#include "image-preview-widget.h"
#include "ui-defines.h"
#include "exit-code-defines.h"

KiranAvatarEditor::KiranAvatarEditor(const QString &srcImagePath, const QString &dstImagePath)
        : KiranTitlebarWindow(){
    initUI(srcImagePath,dstImagePath);
}

KiranAvatarEditor::~KiranAvatarEditor() {

}

void KiranAvatarEditor::initUI(const QString &srcImagePath,
                               const QString &dstImagePath) {
    setTitle(tr("Avatar Editor"));
    setResizeable(false);
    setFixedSize(500, 570);
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);

    //主布局
    m_layout = new QVBoxLayout(getWindowContentWidget());
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    //预览图片
    m_imagePreview = new ImagePreviewWidget(this);
    m_imagePreview->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
    m_imagePreview->setSrcImagePath(srcImagePath);
    m_imagePreview->setDstImagePath(dstImagePath);
    m_layout->addWidget(m_imagePreview);

    ///重新上传
    auto *toolBtnWidget = new QWidget(this);
    toolBtnWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *toolBtnLayout = new QHBoxLayout(toolBtnWidget);

    //分割线
    auto *dividingLine = new QWidget(this);
    dividingLine->setObjectName("dividingLine");
    dividingLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dividingLine->setFixedHeight(1);
    m_layout->addWidget(dividingLine);

    //按钮
    m_buttonWidget = new QWidget(getWindowContentWidget());
    m_buttonWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_buttonWidget->setContentsMargins(-1, 15, -1, 15);
    m_layout->addWidget(m_buttonWidget);
    auto *buttonLayout = new QHBoxLayout(m_buttonWidget);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(-1, -1, -1, -1);
    auto *spacerItem_1 = new QSpacerItem(10, 10,
                                                QSizePolicy::Expanding,
                                                QSizePolicy::Preferred);
    buttonLayout->addItem(spacerItem_1);

    m_btnConfrim = new QPushButton(this);
    m_btnConfrim->setObjectName("btn_confirm");
    m_btnConfrim->setFixedSize(100, 50);
    m_btnConfrim->setText(tr("Confirm"));
    buttonLayout->addWidget(m_btnConfrim);
    connect(m_btnConfrim,&QPushButton::clicked,[this](){
        if( m_imagePreview->saveAvatar() ){
            qApp->exit(EXIT_CODE_SUCCESS);
        }else{
            qApp->exit(EXIT_CODE_SAVE_FAILED);
        }
    });

    auto *spacerItem_2 = new QSpacerItem(80, 20,
                                                QSizePolicy::Fixed,
                                                QSizePolicy::Preferred);
    buttonLayout->addItem(spacerItem_2);

    m_btnCancel = new QPushButton(this);
    m_btnCancel->setObjectName("btn_cancel");
    m_btnCancel->setFixedSize(100, 50);
    m_btnCancel->setText(tr("Cancel"));
    connect(m_btnCancel,&QPushButton::clicked,[this](){
        //TODO:询问，退出不会保存
        qApp->exit(EXIT_CODE_CANCEL);
    });
    buttonLayout->addWidget(m_btnCancel);

    QSpacerItem *spacerItem_3;
    spacerItem_3 = new QSpacerItem(10, 10,
                                   QSizePolicy::Expanding,
                                   QSizePolicy::Preferred);
    buttonLayout->addItem(spacerItem_3);
}

void KiranAvatarEditor::closeEvent(QCloseEvent *event) {
    qApp->exit(EXIT_CODE_CANCEL);
    QWidget::closeEvent(event);
}
