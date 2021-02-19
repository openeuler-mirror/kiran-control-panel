#ifndef LOADANIMATIONWIDGET_H
#define LOADANIMATIONWIDGET_H

#include <QWidget>
#include <QTimer>

class LoadAnimationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadAnimationWidget(QWidget *parent = nullptr);
    ~LoadAnimationWidget() override = default;


protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual QSize sizeHint() const override;


public slots:
    /* restart Animation */
    void resetAnimation();

    /* stop Animation */
    void stopAnimation();

    /* resume Animation */
    void resumeAnimation();

private slots:
    void renderNextFrame();
private:
    QTimer timer;
    int index;
};

#endif // LOADANIMATIONWIDGET_H
