#ifndef KIRANCPANELMOUSEWIDGET_H
#define KIRANCPANELMOUSEWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class KiranCPanelMouseWidget;
}

class KiranSwitchButton;
class KiranListItem;
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

public slots:
    void setCurrentPageWhenItemClicked(QListWidgetItem* item);

private:
    Ui::KiranCPanelMouseWidget *ui;
    KiranSwitchButton* m_naturalScrollSwitch = nullptr;
    KiranSwitchButton* m_middleEmulationSwitch = nullptr;
    KiranListItem* m_mouseListItem;
    KiranListItem* m_touchpadListItem;
};

#endif // KIRANCPANELMOUSEWIDGET_H
