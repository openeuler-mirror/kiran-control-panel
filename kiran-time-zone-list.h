#ifndef KIRANTIMEZONELIST_H
#define KIRANTIMEZONELIST_H

#include <QWidget>
#include <QList>

#include "kiran-time-zone-item.h"
#include "SystemDaemonTimeDate.h"

namespace Ui {
class KiranTimeZoneList;
}

class KiranTimeZoneList : public QWidget
{
    Q_OBJECT

public:
    explicit KiranTimeZoneList(QWidget *parent = nullptr);
    ~KiranTimeZoneList();

    bool initAllTimeZone();
    void switchToAllTimeZone();
    //定时一段时间查询，避免输入过快
    void addSearchTimeoutTask(const QString& keyword);

    void reset();
Q_SIGNALS:
    void sigHeightChanged(int height);
    void sigSeletedZoneInfoChanged(const QString& zoneID);

private Q_SLOTS:
    void slotTimeZoneItemClicked();

private:
    void adjustHeight();
    void search();

    /**
     * @brief 清理已构建的搜索时区控件
     */
    void cleanFilterTimeZoneWidget();

    /**
     * @brief 依据m_filtedZoneInfoList重新构建搜索到的时区控件
     */
    void rebuildFilterTimeZoneWidget();
protected:
    virtual void timerEvent(QTimerEvent *ev) Q_DECL_OVERRIDE;

private:
    Ui::KiranTimeZoneList *ui;
    TimeZoneList m_filtedZoneInfoList;
    QString m_seletedZoneID;
    QString m_keyword;
    int m_timeoutSearchTaskID;
};

#endif // KIRANTIMEZONELIST_H
