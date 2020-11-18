#ifndef KIRANCONTROLPANEL_H
#define KIRANCONTROLPANEL_H

#include "kiran-control-panel-global.h"
#include <QWidget>

class KiranModeClassWgt;
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
    KiranModeClassWgt *m_classWgt;
    QMap<int, ModelClass> m_data;
};

#endif // KIRANCONTROLPANEL_H
