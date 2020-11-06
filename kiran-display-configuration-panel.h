#ifndef KIRANDISPLAYCONFIGURATIONPANEL_H
#define KIRANDISPLAYCONFIGURATIONPANEL_H

#include <QFrame>
#include <QMap>

namespace Ui {
class KiranDisplayConfigurationPanel;
}

class KiranDisplayConfigurationPanel : public QFrame
{
    Q_OBJECT

public:
    explicit KiranDisplayConfigurationPanel(QWidget *parent = 0);
    ~KiranDisplayConfigurationPanel();

    QVariantMap getData();
    void setData(const QVariantList &list, const bool &clearChecked=false);

Q_SIGNALS:
    void buttonChecked(QString monitorPath);

private slots:
    void paintEvent(QPaintEvent *event);
    void on_pushButton_left_clicked();
    void on_pushButton_vertical_clicked();
    void on_pushButton_right_clicked();
    void on_pushButton_identifying_clicked();

private:
    Ui::KiranDisplayConfigurationPanel *ui;
    QVariantList m_textList;
    QMap<QString, QSize> m_textSize;
    QString tt;
};

#endif // KIRANDISPLAYCONFIGURATIONPANEL_H
