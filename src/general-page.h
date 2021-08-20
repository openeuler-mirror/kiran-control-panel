#ifndef GENERALPAGE_H
#define GENERALPAGE_H

#include <QWidget>

namespace Ui
{
class GeneralPage;
}

class GeneralPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralPage(QWidget *parent = 0);
    ~GeneralPage();
    virtual QSize sizeHint() const override;

private:
    void initUI();

private:
    Ui::GeneralPage *ui;
};

#endif  // GENERALPAGE_H
