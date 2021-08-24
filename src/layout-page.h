#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include <QMap>
#include <QWidget>
namespace Ui
{
class LayoutPage;
}

class KSKKeyboardProxy;
class LayoutPage : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutPage(QWidget *parent = 0);
    ~LayoutPage();

private:
    void initUI();
    void getValidLayout();
    int getJsonValueFromString(QString jsonString);
    void createLayoutItem();

private:
    Ui::LayoutPage *ui;
    QSharedPointer<KSKKeyboardProxy> m_keyboardInterface;
    QMap<QString, QString> m_layoutMap;
    QStringList m_layoutList;
};

#endif  // LAYOUTPAGE_H
