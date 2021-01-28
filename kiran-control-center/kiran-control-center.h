#pragma once

#include "kiran-control-center-global.h"
#include <QWidget>

class KiranModuleClassWidget;
using namespace KiranControlCenterGlobal;
namespace Ui {
class KiranControlCenter;
}

class QFrame;
class KiranControlCenter : public QWidget
{
    Q_OBJECT

public:
    explicit KiranControlCenter(QWidget *parent = 0);
    ~KiranControlCenter();
    QStringList completerKeys();

public slots:
    void onSearch(const QString &request);

private:
    void resizeEvent(QResizeEvent *event);
    void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::KiranControlCenter *ui;
    KiranModuleClassWidget *m_classWgt;
    QListWidgetItem *m_curClassListItem;
    QMap<int, ModuleClass> m_data;
    QString m_request;
};

