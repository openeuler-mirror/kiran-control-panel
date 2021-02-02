#ifndef KIRANDISPLAYCONFIGURATION_H
#define KIRANDISPLAYCONFIGURATION_H

#include "kiran-display-config-global.h"
#include "kiran-display-module-base.h"
#include <QWidget>
#include <QComboBox>

using namespace KiranDisplayConfigGlobal;

namespace Ui {
class KiranDisplayConfiguration;
}

class QButtonGroup;
class KiranDisplayConfiguration : public QWidget, public KiranDisplayModuleBase
{
    Q_OBJECT

public:
    explicit KiranDisplayConfiguration(QWidget *parent = 0);
    ~KiranDisplayConfiguration();
    bool hasUnsavedOptions() override;

Q_SIGNALS:
    void sigClose();

public Q_SLOTS:
    void on_pushButton_ok_clicked();
    void on_pushButton_extra_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_extra_cancel_clicked();

private slots:
    void onTabChanged(int index, const bool &checked);
    void onScreenItemChecked(QString monitorPath);
    void on_comboBox_resolving_currentTextChanged(const QString &arg1);
    void on_comboBox_extra_resolving_currentTextChanged(const QString &arg1);
    void onDbusPropertiesChanged(QDBusMessage mes);
    //void onDbusPropertiesChanged(QDBusMessage mes);
    void on_pushButton_enabled_toggled(bool checked);

private:
    QList<DisplayModesStu> intersectionMonitorModes();
    QMap<int, QPair<QSize, QList<int> > > getResolutionFromModes(const QList<DisplayModesStu> &modes);
    DisplayModesStu curIntersectionMonitorMode();
    void setMonitorProperty(const QString &monitorPath, const QVariantMap &map);
    //
    void initCopeMode();
    void initExtraMode(const bool &clearChecked=false);
    void initComboBoxResolution(QComboBox *comboBox, const QMap<int, QPair<QSize, QList<int> > > &map);
    void initComboBoxRefreshRate(QComboBox *comboBox, const QList<int> &refreshRateList);
    void initExtraComboBoxResolution(QComboBox *comboBox, const QMap<int, QPair<QSize, QList<int> > > &map);
    void initExtraComboBoxRefreshRate(QComboBox *comboBox, const QList<int> &refreshRateList);
    void curExtraData2Cache();
    void showMessageBox();
    void selectResolutionComboboxItem(QComboBox *comboBox, const int &w, const int &h);
    void selectRefreshRateComboboxItem(QComboBox *comboBox, const int &r);
    bool isCopyMode();
    bool extraPrimaryBtnStatus(const bool &onlyEnableScreen, const bool &enable);
    //UI创建之后，加载数据的入口
    void refreshWidget();
    //start用于检查是否修改
    QVariantMap getCopyModeUiData();
    QVariantMap getExtraModeUiData();

private:
    Ui::KiranDisplayConfiguration *ui;
    QString m_curMonitorPath;
    QString m_primaryMonitorName;
    QMap<QString, QVariantMap> m_extraData;
    QStringList m_dbusConnectList;
    QButtonGroup *m_btnGroup;
    //start用于检查是否修改
    QVariantMap m_copyModeSavedData;
    QVariantMap m_extraModeSavedData;
    //end
    QStringList m_listMonitors;//用于处理虚拟机中，没有勾选显示器，xrandr返回两个显示器，但是部分显示器的dbus却无法调用的情况。
    bool m_dbusPropertiesChangedBlock;
};

#endif // KIRANDISPLAYCONFIGURATION_H
