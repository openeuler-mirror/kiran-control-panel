#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include <QMap>
#include <QVBoxLayout>
#include <QWidget>
namespace Ui
{
class LayoutPage;
}

class ChooseItem;
class KeyboardBackEndProxy;
class LayoutPage : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutPage(QWidget *parent = 0);
    ~LayoutPage();

private:
    void initUI();
    void getValidLayout();
    int getJsonValueFromString(QString jsonString);
    void createLayoutItem();
    void updateLayout();

signals:
    void layoutSelectChanged(QString layoutName);

public slots:
    void deleteLayout(QString deletedLayout);
    void setEditMode();
    bool addLayout(QString layoutName);
    void chooseItemClicked();

private:
    Ui::LayoutPage *ui;
    QSharedPointer<KeyboardBackEndProxy> m_keyboardInterface;
    QMap<QString, QString> m_layoutMap;
    QList<ChooseItem *> m_itemList;
    QStringList m_layoutList;
    QString m_layout;

    //    ChooseItem *m_firstItem;
    QVBoxLayout *m_vLayout;
    bool m_editFlag = false;
};

#endif  // LAYOUTPAGE_H
