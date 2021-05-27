//
// Created by lxh on 2021/5/17.
//

#ifndef KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
#define KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_

#include <QLineEdit>
#include <QItemDelegate>

//TODO:完成控制面板搜索

class CPanelSearchDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CPanelSearchDelegate(QObject* parent = nullptr);
    ~CPanelSearchDelegate();

public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class QStandardItemModel;
class QCompleter;
class QListView;
class CPanelSearchEdit : public QLineEdit
{
    Q_OBJECT
public:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit CPanelSearchEdit(QWidget* parent = nullptr);
    ~CPanelSearchEdit();

private:
    void init();
    QStandardItemModel* buildSearchModel();
    void setSearchPopupVisible(bool searchPopupVisible);

signals:

    void sigJump(int categoryIdx,int subItemIdx);
private:
    QStandardItemModel* m_searchModel = nullptr;
    QCompleter* m_completer = nullptr;
};

#endif  //KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
