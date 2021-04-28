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

class ComKylinsecKiranSessionDaemonMouseInterface;
class ComKylinsecKiranSessionDaemonTouchPadInterface;
class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
class KiranCPanelMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCPanelMouseWidget(QWidget *parent = 0);
    ~KiranCPanelMouseWidget();

public:
    bool initUI();

private:
    bool connectDbus();
    void initPageMouseUI();
    void initPageTouchPadUI();
    void addComboBoxItem();
    void addSidebarItem(QString,QString);
    void setDisableWidget(bool);

public slots:
    void setCurrentPageWhenItemClicked(QListWidgetItem* item);
    void onSliderReleased();
    void onDisabelTouchPadToggled(bool disabled);

private:
    Ui::KiranCPanelMouseWidget *ui;
    ComKylinsecKiranSessionDaemonMouseInterface *m_mouseInterface;
    ComKylinsecKiranSessionDaemonTouchPadInterface *m_touchPadInterface;
    KiranListItem* m_mouseListItem;
    KiranListItem* m_touchpadListItem;
    QList<QComboBox* > m_comboBoxList;
    QList<KiranSwitchButton*> m_checkBoxList;
    QList<QLabel*> m_labelList;
    bool m_mouseLeftHand = false;
    bool m_mouseNaturalScroll = false;
    bool m_middleEmulationEnabled = false;
    double m_mouseMotionAcceleration = 0.0;

    bool m_disabelWhileTyping = true;
    bool m_touchPadLeftHand = false;
    bool m_touchPadNaturalScroll = false;
    bool m_tapToClick = true;
    bool m_touchPadEnabled = true;
    double m_touchPadMotionAcceleration = 0.0;
    int m_clickMethod = 0;
    int m_scrollMethod = 0;
};

#endif // KIRANCPANELMOUSEWIDGET_H
