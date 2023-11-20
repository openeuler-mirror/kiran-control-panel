#include <QWidget>
#include <QPropertyAnimation>

class QPropertyAnimation;

namespace Ui {
class KiranCollapse;
}

class KiranCollapse : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCollapse(QWidget *parent = nullptr);
    KiranCollapse(bool defaultIsExpand, const QString& title, QWidget* expandSpaceWidget, QWidget *parent = nullptr);
    ~KiranCollapse() override;

    void setTitle(const QString &title);
    void addTopBarWidget(QWidget* widget);
    void setTobBarFixedHeight(int height);

    bool getIsExpand() const;
    void setIsExpand(bool isExpanded);
    void setMaximumExpansionHeight(int maxExpandHeight);
    void setExpandSpaceAnimation();
    void addExpansionSpaceWidget(QWidget *widget);
    void delExpansionSpaceWidget(int index);
    void delExpansionSpaceWidget(const QString& widgetName);
    void delAllExpansionSpaceWidget();

signals:
    void expandSpaceExpanded();
    void expandSpaceCollapsed();

public slots:
    void expand();
    void collapse();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void changeExpansionSpaceState();

private:
    void init();

private:
    Ui::KiranCollapse *ui;

    bool m_isExpanded = false;
    // 展开高度
    int m_maximumExpansionSpaceHeight = 400;
    int m_expansionSpaceContentMarginLeft = 10;
    int m_expansionSpaceContentMarginTop = 10;
    int m_expansionSpaceContentMarginRight = 10;
    int m_expansionSpaceContentMarginBottom = 10;
    int m_radius = 6;
    bool m_drawBackground = true;
    QPropertyAnimation* m_animationForES{};
};
