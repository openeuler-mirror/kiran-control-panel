#ifndef SCROLLPICKER_H
#define SCROLLPICKER_H

#include <QWidget>
#include <QVariant>
#include <QColor>
#include <QPersistentModelIndex>
#include <QPropertyAnimation>

class QStandardItemModel;

///FIXME:model的大小未做限制
class ScrollPicker : public QWidget
{
    Q_OBJECT
    ///当前模型条数
    Q_PROPERTY(int    count             READ count)
    ///当前值距离y轴中心偏移距离
    Q_PROPERTY(int    deviation         READ deviation         WRITE setDeviation)
    ///显示条数
    Q_PROPERTY(int    showCount         READ showCount         WRITE setShowCount)
    ///是否循环显示
    Q_PROPERTY(bool   loop              READ loop              WRITE setLoop)
    ///中心字体大小(pt)(会随着deviation偏移大字体而更小)
    Q_PROPERTY(int    fontSize          READ fontSize          WRITE setFontSize)
    ///字体颜色
    Q_PROPERTY(QColor fontColor         READ fontColor         WRITE setFontColor)
    ///当前数据来自模型的第几列
    Q_PROPERTY(int    modelColumn       READ modelColumn       WRITE setModelColumn)
    ///悬浮字体颜色
    Q_PROPERTY(QColor hoverColor        READ hoverColor        WRITE setHoverColor)
    ///悬浮之后当前值的字体增大多少pt
    Q_PROPERTY(int    hoverIncreaseFont READ hoverIncreaseFont WRITE setHoverIncreaseFont )
public:
    explicit ScrollPicker(QWidget *parent = nullptr);
    ~ScrollPicker();

    int count() const;
    int deviation() const;
    int showCount() const;
    bool loop() const;
    int fontSize() const;
    QColor fontColor() const;
    int modelColumn() const;

    QString currentText();
    int currentIndex();
    QVariant currentData(int role=Qt::UserRole);

    QString itemText(int index);
    QVariant itemData(int index,int role=Qt::UserRole);

    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    void insertItem(int index, const QString &text,
                           const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);

    void removeItem(int index);

    void setItemText(int index, const QString &text);
    void setItemData(int index, const QVariant &value, int role = Qt::UserRole);

    QColor hoverColor() const;
    int hoverIncreaseFont() const;

private:
    void init();
    void emitCurrentIndexChangedSignals();

    QModelIndex getNextModelIndex(const QModelIndex &index);
    QModelIndex getPrevModelIndex(const QModelIndex &idnex);

    bool currentIndexMoveUpper(bool needUpdate);
    bool currentIndexMoveDown(bool needUpdate);

    void homing();

Q_SIGNALS:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString& text);

public Q_SLOTS:
    void clean();
    void setCurrentIndex(int index, bool needUpdate);
    void setCurrentText(const QString &text, bool needUpdate);
    void setDeviation(int devitation);
    void setShowCount(int showCount);
    void setLoop(bool loop);
    void setFontSize(int fontSize);
    void setFontColor(QColor fontColor);
    void setModelColumn(int modelColumn);
    void setHoverColor(QColor hoverColor);
    void setHoverIncreaseFont(int hoverIncreaseFont);

private Q_SLOTS:
    void modelDataChanged(const QModelIndex& topleft,
                          const QModelIndex& bottomright,
                          const QVector<int> &roles);
    void modelRowsInserted(const QModelIndex &parent,
                           int first, int last);
    void modelRowsRemoved(const QModelIndex &parent,
                          int first, int last);
    void modelReset();
    void updateIndexBeforeChange();

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    QStandardItemModel* m_model;
    QPersistentModelIndex m_rootIndex;
    QPersistentModelIndex m_currentIndex;
    QPropertyAnimation* m_animation;
    int m_indexBeforeChange;
    bool m_pressed;
    int m_pressedY;
    int m_deviation;
    int m_showCount;
    bool m_loop;
    int m_fontSize;
    QColor m_fontColor;
    QEasingCurve m_fontAlphaEasingCurve;
    int m_modelColumn;
    bool m_hoverd;
    QColor m_hoverColor;
    int m_hoverIncreaseFont;
};

#endif // SCROLLPICKER_H
