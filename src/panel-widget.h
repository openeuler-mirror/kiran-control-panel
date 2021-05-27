#ifndef __PANEL_CONTENT_WIDGET_H__
#define __PANEL_CONTENT_WIDGET_H__

#include <QWidget>

class CategoryWidget;
namespace Ui
{
class PanelWidget;
}

class PanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PanelWidget(QWidget *parent = 0);
    ~PanelWidget();

private:
    void init();

private:
    void resizeEvent(QResizeEvent *event);

private slots:
    void handleCurrentCategoryChanged(int curCategoryIdx,
                                      int prevCategoryIdx);

private:
    Ui::PanelWidget *ui;
    CategoryWidget *m_categoryWidget;
    int m_currentCategoryIndex = -1;
};

#endif  //__PANEL_CONTENT_WIDGET_H__
