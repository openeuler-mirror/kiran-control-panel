#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include <QWidget>

namespace Ui {
class LayoutPage;
}

class LayoutPage : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutPage(QWidget *parent = 0);
    ~LayoutPage();

private:
    Ui::LayoutPage *ui;
};

#endif // LAYOUTPAGE_H
