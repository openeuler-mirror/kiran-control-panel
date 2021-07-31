#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QWidget>
#include <QTimer>
#include <QScrollArea>

class FlowLayout;
class KiranImageItem;
class ScrollContainer;
class ImageSelector: public QWidget
{
    Q_OBJECT
public:    
    explicit ImageSelector(QWidget *parent= nullptr);

    QStringList imageList();

    void addImage(const QString &imagePath,int imageType);
    void addAdditionImage(bool isAdditionImage);

    void removeImage(QString path);

    QString selectedImage();

    void setSelectedImage(QString path,bool);

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
    //virtual QSize sizeHint();
private:
    QStringList m_imageList;
    FlowLayout *m_flowLayout;
    QList<KiranImageItem *> m_itemList;
    QString m_selectedImagePath;
    QTimer m_updateTimer;
    KiranImageItem* m_addImageItem = nullptr;
    KiranImageItem* m_customImageItem = nullptr;
    int m_selectorType = -1;
    ScrollContainer *m_container;
    QScrollArea *scrollArea;
};

#endif // IMAGESELECTOR_H
