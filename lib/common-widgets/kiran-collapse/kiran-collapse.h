/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#include <QWidget>

class QPropertyAnimation;

namespace Ui
{
class KiranCollapse;
}

class KiranCollapse : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCollapse(QWidget* parent = nullptr);

    ~KiranCollapse() override;

    /**
     * @brief 通过有参构造实例化 KiranCollapse
     * @param 默认是否展开, 文本标题, 扩展区控件, 父控件
     */
    KiranCollapse(bool defaultIsExpand, const QString& title, QWidget* expandSpaceWidget, QWidget* parent = nullptr);

    /**
     * @brief 设置 kiran-collapse 的标题
     * @param 标题文本
     */
    void setTitle(const QString& title);

    /**
     * @brief 添加一个控件到顶栏
     * @param QWidget控件，加入方式为：每次加入的控件会放在最右边图标的左侧
     */
    void addTopBarWidget(QWidget* widget);

    /**
     * @brief 设置顶栏的固定高度
     * @param height 高度（默认为45）
     */
    void setTobBarFixedHeight(int height);

    /**
     * @brief 设置扩展区内部间距
     * @param left, top, right, bottom
     */
    void setExpansionMargin(int left, int top, int right, int bottom);

    /**
     * @brief 设置顶栏内部间距
     * @param left, top, right, bottom
     */
    void setTopBarMargin(int left, int top, int right, int bottom);

    /**
     * @brief 设置顶栏控件间距
     * @param spacing
     */
    void setTopBarSpacing(int spacing);

    /**
     * @brief 获取 kiran-collapse 是否展开
     * @return bool
     */
    bool getIsExpand() const;

    /**
     * @brief 设置 kiran-collapse 是否展开
     * @param bool
     */
    void setIsExpand(bool isExpanded);

    /**
     * @brief 设置扩展区最大展开高度
     * @param maxExpandHeight 最大展开高度 (默认为400)
     */
    void setMaximumExpansionHeight(int maxExpandHeight);

    /**
     * @brief 添加控件到扩展区
     * @param widget - 要加入到扩展区的控件
     */
    void addExpansionSpaceWidget(QWidget* widget);

    /**
     * @brief 根据索引删除扩展区的控件
     * @param index - 扩展区内控件从上到下的索引
     */
    void delExpansionSpaceWidget(int index);

    /**
     * @brief 根据控件对象名称删除控件
     * @param widgetName
     */
    void delExpansionSpaceWidget(const QString& widgetName);

    /**
     * @brief 清除扩展区所有控件
     */
    void delAllExpansionSpaceWidget();

signals:
    void expandSpaceExpanded();
    void expandSpaceCollapsed();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void changeExpansionState();

private:
    void init();
    void expand();
    void collapse();

private:
    Ui::KiranCollapse* ui;

    // 是否展开
    bool m_isExpanded = false;
    // 展开最大高度
    int m_maximumExpansionSpaceHeight = 400;

    // 扩展区内边距 (默认12)
    int m_expansionMarginLeft = 12;
    int m_expansionMarginTop = 12;
    int m_expansionMarginRight = 12;
    int m_expansionMarginBottom = 12;

    // 外部圆角
    int m_radius = 6;
    bool m_drawBackground = true;
    // 扩展区展开搜索动画
    QPropertyAnimation* m_animationForES{};

    // 顶栏标题
    QString m_topBarTitle;
    // 扩展区控件
    QWidget *m_esWidget;
};
