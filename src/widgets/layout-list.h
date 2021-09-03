#ifndef LAYOUTLIST_H
#define LAYOUTLIST_H

#include <QListWidget>
#include <QTimer>
#include <QWidget>
namespace Ui
{
class LayoutList;
}

class LayoutItem;
class LayoutList : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutList(QWidget *parent = 0);
    ~LayoutList();
    void setCountryList(QStringList layoutList);
    QString getSelectedCountry();

public slots:
    void itemClicked();

signals:
    void itemChanged(QString countryName);
    void heightChanged(int height);

private:
    void search();
    void clearFilterItems();
    LayoutItem *addItems(QListWidget *parent, QString text);
    void adjustSize();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::LayoutList *ui;
    QTimer *m_timer = nullptr;
    QStringList m_filterList;
    QStringList m_lists;
    QString m_countryName;
};

#endif  // LAYOUTLIST_H
