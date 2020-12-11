#ifndef KIRANCONTROLPANEL_H
#define KIRANCONTROLPANEL_H

#include "kiran-control-panel-global.h"
#include <QWidget>

class KiranModuleClassWidget;
using namespace KiranControlPanelGlobal;
namespace Ui {
class KiranControlPanel;
}

class QFrame;
class KiranControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit KiranControlPanel(QWidget *parent = 0);
    ~KiranControlPanel();
    QStringList completerKeys();

public slots:
    void onSearch(const QString &request);

private:
    void resizeEvent(QResizeEvent *event);
    void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::KiranControlPanel *ui;
    KiranModuleClassWidget *m_classWgt;
    QListWidgetItem *m_curClassListItem;
    QMap<int, ModuleClass> m_data;
    QString m_request;
};

#endif // KIRANCONTROLPANEL_H
