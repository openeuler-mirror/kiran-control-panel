#ifndef KIRANCONTROLPANEL_H
#define KIRANCONTROLPANEL_H

#include "kiran-control-panel-global.h"
#include <QWidget>

class KiranModuleClassListWidget;
using namespace KiranControlPanelGlobal;
namespace Ui {
class KiranControlPanel;
}

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

private:
    Ui::KiranControlPanel *ui;
    KiranModuleClassListWidget *m_classWgt;
    QListWidgetItem *m_curClassListItem;
    QMap<int, ModuleClass> m_data;
};

#endif // KIRANCONTROLPANEL_H
