#ifndef GENERALPAGE_H
#define GENERALPAGE_H

#include <QTimer>
#include <QWidget>

namespace Ui
{
class GeneralPage;
}

class KSKKeyboardProxy;
class GeneralPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralPage(QWidget *parent = 0);
    ~GeneralPage();
    virtual QSize sizeHint() const override;

private:
    void initUI();
    void initComponentValue();
    void setWidgetsStatus(bool status);
    QSharedPointer<KSKKeyboardProxy> m_keyboardInterface;

private:
    Ui::GeneralPage *ui;
    QTimer *m_timer = nullptr;
    bool m_repeateEnabled = false;
    qint32 m_delay;
    qint32 m_interval;
};

#endif  // GENERALPAGE_H
