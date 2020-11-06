#ifndef KIRANDISPLAYCONFIGURATION_H
#define KIRANDISPLAYCONFIGURATION_H

#include "kiran-display-config-global.h"
#include <QWidget>
#include <QComboBox>

using namespace KiranDisplayConfigGlobal;

namespace Ui {
class KiranDisplayConfiguration;
}

class KiranDisplayConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit KiranDisplayConfiguration(QWidget *parent = 0);
    ~KiranDisplayConfiguration();

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

private:
    Ui::KiranDisplayConfiguration *ui;
    QString m_curMonitorPath;
    QString m_primaryMonitorName;
    QMap<QString, QVariantMap> m_extraData;
    QStringList m_dbusConnectList;
};

#endif // KIRANDISPLAYCONFIGURATION_H
