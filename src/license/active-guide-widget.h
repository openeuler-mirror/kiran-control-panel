#ifndef ACTIVEGUIDEWIDGET_H
#define ACTIVEGUIDEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include "show-qrcode.h"
#include "active-list-item.h"
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include "load-animation-widget.h"
namespace Ui {
class ActGuideWidget;
}

/**
 * @brief  ActGuideWidget 激活向导窗口
 */
class ActGuideWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    enum {Page_Start, Page_Manual,Page_Key,Page_Online,Page_Activating,Page_Successful,Page_Failed};
    explicit ActGuideWidget();
    ~ActGuideWidget();

private:
    void iniUI();
    ActListItem* createActiveItem(const QString &text);
    bool getMachineCode();
    void sleep(int);
    void hideLabelPointsDefault();
    void setLabelPointsVisible(bool);
    void getJsonValueFromString(const QString jsonString);

signals:
    void systemIsActived(bool actived);
    void sig_showQRCodeWgt(QPoint oPoint);

public slots:
    //void get_page_index();
    void handleNextButton();
    void handleActiveButton();
    void handleBackButton();
    void HandleCloseButton();
    void getLicenseStatus(bool isRegister);
    void popupQRCode(QPoint oPoint);
    void setManualLineEditStatus();
    void setKeyLineEditStatus();

protected:
    bool eventFilter(QObject *target, QEvent *e);

private:
    Ui::ActGuideWidget *ui;
    ShowQRCode * showQRCode;
    LoadAnimationWidget *loadAnimation;
    ActListItem* listItemMode;
    ActListItem* listItemActive;
    ActListItem* listItemComplete;
    QString mc_code;
    bool activedFlag;
    bool byManual;
    bool byKey;
    bool byOnline;
};

#endif // ACTIVEGUIDEWIDGET_H
