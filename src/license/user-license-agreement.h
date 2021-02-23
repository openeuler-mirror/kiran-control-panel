#ifndef USERLICENSEAGREEMENT_H
#define USERLICENSEAGREEMENT_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

namespace Ui {
class UserlicenseAgreement;
}

class UserlicenseAgreement : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit UserlicenseAgreement();
    ~UserlicenseAgreement();
    QString getEulaText();

private:
    void initUI();
    void addText();

private:
    Ui::UserlicenseAgreement *ui;
};

#endif // USERLICENSEAGREEMENT_H
