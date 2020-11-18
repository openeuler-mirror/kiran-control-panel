#ifndef KIRANMODEITEMWGT_H
#define KIRANMODEITEMWGT_H

#include "kiran-control-panel-global.h"
using namespace KiranControlPanelGlobal;

#include <QWidget>

namespace Ui {
class KiranModeItemWgt;
}
class QListWidgetItem;
class KiranModeClassWgt;
class KiranModeItemWgt : public QWidget
{
    Q_OBJECT

public:
    explicit KiranModeItemWgt(QWidget *parent = 0);
    ~KiranModeItemWgt();

    void setLeftContentsMargins(const int &leftmargin);
    void setData(QMap<int, ModelItem> &data);
    void setCurItem( QListWidgetItem *item);

public slots:
    void onClassItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private slots:
    void on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);


private:
    Ui::KiranModeItemWgt *ui;
    QWidget *m_curCenterWgt;
};

#endif // KIRANMODEITEMWGT_H
