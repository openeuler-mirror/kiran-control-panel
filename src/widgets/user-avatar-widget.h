#ifndef USERAVATARWIDGET_H
#define USERAVATARWIDGET_H

#include <QWidget>

class UserAvatarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserAvatarWidget(QWidget *parent = nullptr);
    virtual ~UserAvatarWidget() = default;

    void setImage(const QString &path);
    void setDefaultImage();
    void setClickEnable(bool enable);
    bool setHoverImage(const QString &path);
    QString iconPath();

signals:
    void pressed();

public slots:

protected:
    virtual void paintEvent(QPaintEvent *event) override final;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override final;

protected:
    QPixmap generateUserIconFitWidgetSize(const QPixmap &pixmap);

private:
    QPixmap scalePixmapAdjustSize(const QPixmap &pixmap);

private:
    QPixmap m_scaledPixmap;
    QPixmap m_pixmap;
    QPixmap m_hoverPixmap;
    QPixmap m_hoverScaledPixmap;
    bool m_clickedEnable;
    bool m_isEnter;
    bool m_isHover;
    QString m_iconPath;
};

#endif  // USERAVATARWIDGET_H
