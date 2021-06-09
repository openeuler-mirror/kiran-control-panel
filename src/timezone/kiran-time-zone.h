#ifndef KIRANTIMEZONE_H
#define KIRANTIMEZONE_H

#include <QWidget>

namespace Ui {
class KiranTimeZone;
}

#include <QEvent>

class KiranTimeZone : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool editHasFocus READ editHasFocus WRITE setEditHasFocus NOTIFY editHasFocusChanged)
public:
    explicit KiranTimeZone(QWidget *parent = nullptr);
    ~KiranTimeZone();

    bool editHasFocus() const;
    virtual QSize sizeHint() const override;

    bool save();

    void reset();

    Q_INVOKABLE void scrollToCurrent();
private:
    void initUI();

public slots:
    void setEditHasFocus(bool editHasFocus);

signals:
    void editHasFocusChanged(bool editHasFocus);

protected:
    virtual bool event(QEvent *event) override {
        if(event->type()==QEvent::ShowToParent){
            scrollToCurrent();
        }
        return QWidget::event(event);
    }

    virtual void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::KiranTimeZone *ui;
    bool m_editHasFocus;;
    QString m_selectedZoneID;
};

#endif // KIRANTIMEZONE_H
