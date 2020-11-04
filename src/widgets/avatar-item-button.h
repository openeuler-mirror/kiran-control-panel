#ifndef AVATARITEMBUTTON_H
#define AVATARITEMBUTTON_H

#include <QObject>
#include <QAbstractButton>

class AvatarItemButton : public QAbstractButton
{
    Q_OBJECT

public:
    AvatarItemButton(QWidget* parent = nullptr);
    virtual ~AvatarItemButton();
    void setIcon(const QString &iconPath);
    QString iconPath();

protected:
    void setIcon(const QIcon &icon);
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    QRect m_realRect;
    double m_radius;
    bool m_mouseEnter;
    bool m_hover;
    QString m_iconPath;
};

#endif // AVATARITEMBUTTON_H
