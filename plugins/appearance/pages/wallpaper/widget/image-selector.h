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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QScrollArea>
#include <QTimer>
#include <QWidget>

class FlowLayout;
class ImageItem;
class ScrollContainer;
class ImageSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSelector(QWidget *parent = nullptr);

    QStringList imageList();

    void addImage(const QString &imagePath, int imageType);
    void addAdditionImage(bool isAdditionImage);

    void removeImage(QString path);

    QString selectedImage();

    void setSelectedImage(QString path, bool);

    void moveAdditionItemToEnd();

    void setSelectorType(int type);

private:
    void initUI();
    bool isImageExisted(QString path);

signals:
    void selectedImageChanged(int selectorType, QString path);
    void deleteImage(QString deletedPath);
    void addNewImage();

public slots:
    /* 更新可见的Item图片 */
    void updateImageItem();
    void handlerImageItemSelectedChanged();
    void handlerImageDelete(QString imagePath);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QStringList m_imageList;
    FlowLayout *m_flowLayout;
    QList<ImageItem *> m_itemList;
    QString m_selectedImagePath;
    QTimer m_updateTimer;
    ImageItem *m_addImageItem = nullptr;
    ImageItem *m_customImageItem = nullptr;
    int m_selectorType = -1;
    ScrollContainer *m_container;
    QScrollArea *scrollArea;
};

#endif  // IMAGESELECTOR_H
