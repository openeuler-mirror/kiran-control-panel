#ifndef KIRANMODECLASSWGT_H
#define KIRANMODECLASSWGT_H
#include "kiran-control-panel-global.h"
//#include "kiran-mode-class-item.h"
#include "kiran-mode-class-listwidget-item-widget.h"
using namespace KiranControlPanelGlobal;

#include <QListWidget>
#include <QButtonGroup>

class KiranModeClassListWidget : public QListWidget
{
    Q_OBJECT
    bool eventFilter(QObject *obj, QEvent *event);
public:
    explicit KiranModeClassListWidget(QWidget *parent = nullptr);
    void setData(QMap<int, ModelClass> *data);

signals:

public slots:

    void setIconMode();
    void setListMode();
    int iconModeWd();
    int textModeWd();

private:
    QString styleSheetStr();
    void initTimer();

    void onCurrentItemChanged(QListWidgetItem *item);

private:
    QHash<QListWidgetItem *, KiranModeClassListWidgetItemWidget *> m_btns;
    int m_step;
    QTimer *m_timer;
    bool m_showText;
    QButtonGroup *m_btnGroup;
};

#endif // KIRANMODECLASSWGT_H
