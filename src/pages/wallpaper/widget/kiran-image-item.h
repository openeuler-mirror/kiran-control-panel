/**
 *@file  kiran-image-selector-item.h
 *@brief 
 *@auth  liuxinhao <liuxinhao@kylinos.com.cn>
 *@copyright (c) 2021 KylinSec. All rights reserved.
 */
#ifndef KIRANIMAGESELECTOR_KIRAN_IMAGE_ITEM_H
#define KIRANIMAGESELECTOR_KIRAN_IMAGE_ITEM_H

#include <QWidget>
#include <QFuture>
#include <QPixmap>
#include <QFutureWatcher>
#include <QtSvg/QSvgRenderer>
#include <QToolButton>
/**
 * @brief KiranImageSelector图片选择器中的图片项
 */
class KiranImageItem : public QWidget {
Q_OBJECT
    Q_PROPERTY(bool isSelected READ isSelected WRITE setIsSelected NOTIFY isSelectedChanged);
public:
    KiranImageItem(QWidget *parent, const QString &path, int imageType);
    ~KiranImageItem();

    QString imagePath();
    void updatePixmap();

    bool isSelected();
    void setIsSelected(bool selected);
signals:
    void isSelectedChanged(bool isSelected);
    void itemIsSelected(bool isAdditionImage);
    void deleteBtnClicked(QString path);

private:
    void createDeleteButton();
    virtual QSize sizeHint() const;

    void drawSelectedIndicator(QPainter &painter);
    void drawHoverIndicator(QPainter &painter);
    void drawMask(QPainter &painter);
    void drawLoadingImage(QPainter &painter);

protected slots:
    void loadPixmapFinished(QString imagePath, QSize imageSize, QPixmap map);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void leaveEvent(QEvent *event) override;

    virtual void enterEvent(QEvent *event) override;

private:
    QString m_imagePath;
    QPair<QSize, QPixmap> m_previewPixmap;
    bool m_isDown;
    bool m_isSelected = false;
    bool m_isHover = false;
    int m_imageType;
    bool m_isAdditionImage = false;
    QToolButton * m_deleteBtn = nullptr;
};


#endif //KIRANIMAGESELECTOR_KIRAN_IMAGE_ITEM_H
