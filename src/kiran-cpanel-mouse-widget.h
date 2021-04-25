#ifndef KIRANCPANELMOUSEWIDGET_H
#define KIRANCPANELMOUSEWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

namespace Ui {
class KiranCPanelMouseWidget;
}

class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
class KiranCPanelMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCPanelMouseWidget(QWidget *parent = 0);
    ~KiranCPanelMouseWidget();

private:
    void initUI();
    KiranListItem* createListItem(QString text);
    void addComboBoxItem();
    void addSidebarItem(QString,QString);
    void setDisableWidget(bool);

public slots:
    void setCurrentPageWhenItemClicked(QListWidgetItem* item);
    void onSliderReleased();
    void onDisabelTouchPadToggled(bool disabled);

private:
    Ui::KiranCPanelMouseWidget *ui;
    KiranListItem* m_mouseListItem;
    KiranListItem* m_touchpadListItem;
    int m_mouseScrollSpeed;
    int m_tpScrollSpeed;
    QList<QComboBox* > m_comboBoxList;
    QList<KiranSwitchButton*> m_checkBoxList;
    QList<QLabel*> m_labelList;

};

#endif // KIRANCPANELMOUSEWIDGET_H
