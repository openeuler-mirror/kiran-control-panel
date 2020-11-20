#ifndef KIRANMODELWIDGET_H
#define KIRANMODELWIDGET_H

#include "kiran-control-panel-global.h"
using namespace KiranControlPanelGlobal;

#include <QWidget>

namespace Ui {
class KiranModelWidget;
}
class QListWidgetItem;
class KiranModeClassListWidget;
class KiranModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranModelWidget(QWidget *parent = 0);
    ~KiranModelWidget();

    void setLeftContentsMargins(const int &leftmargin);
    //多个模块数据合并。
    void setModelsData(QMap<int, ModelItem> &data);
    void setCurModelSubItem( QListWidgetItem *item);
    bool checkHasUnSaved();
    void changeCurModelSubItem(QListWidgetItem *current);

public slots:
    void onSelectedClassItemChanged(QListWidgetItem *current);

private slots:
    void on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void closeCenterWidgetPlugin(QListWidgetItem *current);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::KiranModelWidget *ui;
    QListWidgetItem *m_curItem;
    QWidget *m_curCenterWgt;
    bool m_retracement;
};

#endif // KIRANMODELWIDGET_H
