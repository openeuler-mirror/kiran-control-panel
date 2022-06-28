#include "scroll-picker.h"
#include "scroll-picker.h"

#include <QStandardItemModel>
#include <QtDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QStyleOption>
#include <QEasingCurve>

ScrollPicker::ScrollPicker(QWidget *parent)
    : QWidget(parent),
      m_model(new QStandardItemModel(0,1,this)),
      m_rootIndex(QModelIndex()),
      m_currentIndex(QModelIndex()),
      m_animation(new QPropertyAnimation(this,"deviation",this)),
      m_indexBeforeChange(-1),
      m_pressedY(false),
      m_deviation(0),
      m_showCount(3),
      m_loop(false),
      m_fontSize(16),
      m_fontColor(Qt::white),
      m_fontAlphaEasingCurve(QEasingCurve::OutQuad),
      m_modelColumn(0),
      m_hoverd(false),
      m_hoverColor("#43a3f2"),
      m_hoverIncreaseFont(3)
{
    setAttribute(Qt::WA_Hover);
    init();
}

ScrollPicker::~ScrollPicker()
{

}

int ScrollPicker::count() const
{
    return m_model->rowCount(m_rootIndex);
}

int ScrollPicker::deviation() const
{
    return m_deviation;
}

int ScrollPicker::showCount() const
{
    return m_showCount;
}

bool ScrollPicker::loop() const
{
    return m_loop;
}

int ScrollPicker::fontSize() const
{
    return m_fontSize;
}

QColor ScrollPicker::fontColor() const
{
    return m_fontColor;
}

QString ScrollPicker::currentText()
{
    if( m_currentIndex.isValid() ){
        return m_model->data(m_currentIndex,Qt::DisplayRole).toString();
    }
    return QString();
}

int ScrollPicker::currentIndex()
{
    return m_currentIndex.row();
}

QVariant ScrollPicker::currentData(int role)
{
    return m_currentIndex.data(role);
}

QString ScrollPicker::itemText(int index)
{
    QModelIndex mi = m_model->index(index,m_modelColumn,m_rootIndex);
    return mi.isValid()?m_model->data(mi,Qt::DisplayRole).toString():QString();
}

QVariant ScrollPicker::itemData(int index,
                                int role)
{
    QModelIndex mi = m_model->index(index,m_modelColumn,m_rootIndex);
    return mi.isValid()?m_model->data(mi,role):QVariant();
}

void ScrollPicker::addItem(const QString &text,
                           const QVariant &userData)
{
    insertItem(count(),text,userData);
}

void ScrollPicker::addItems(const QStringList &texts)
{
    insertItems(count(),texts);
}

void ScrollPicker::insertItem(int index,
                              const QString &text,
                              const QVariant &userData)
{
    QStandardItem* item = new QStandardItem(text);
    if(userData.isValid()){
        item->setData(userData,Qt::UserRole);
    }
    m_model->insertRow(index,item);
}

void ScrollPicker::insertItems(int index,
                               const QStringList &texts)
{
    if(texts.isEmpty()){
        return;
    }
    int insertCount = texts.size();
    QList<QStandardItem *> items;
    items.reserve(insertCount);
    QStandardItem *hiddenRoot = m_model->invisibleRootItem();
    for(int i=0;i<insertCount;i++){
        items.insert(i,new QStandardItem(texts.at(i)));
    }
    hiddenRoot->insertRows(index,items);
}

void ScrollPicker::setItemText(int index,
                               const QString &text)
{
    setItemData(index,text,Qt::DisplayRole);
}

void ScrollPicker::setItemData(int index,
                               const QVariant &value,
                               int role)
{
    QModelIndex mi = m_model->index(index,m_modelColumn,m_rootIndex);
    if(mi.isValid()){
        m_model->setData(mi,value,role);
    }
}

QColor ScrollPicker::hoverColor() const
{
    return m_hoverColor;
}

int ScrollPicker::hoverIncreaseFont() const
{
    return m_hoverIncreaseFont;
}

void ScrollPicker::removeItem(int index)
{
    if( index<0 && index>=count() ){
        return;
    }
    m_model->removeRows(index,1,m_rootIndex);
}

int ScrollPicker::modelColumn() const
{
    return m_modelColumn;
}

void ScrollPicker::setDeviation(int deviation)
{
    if(m_deviation == deviation){
        return;
    }
    m_deviation = deviation;
    repaint();
}

void ScrollPicker::setShowCount(int showCount)
{
    if(m_showCount == showCount){
        return;
    }
    m_showCount = showCount;
    repaint();
}

void ScrollPicker::setLoop(bool loop)
{
    if(m_loop == loop){
        return;
    }
    m_loop = loop;
    repaint();
}

void ScrollPicker::setFontSize(int fontSize)
{
    if(m_fontSize == fontSize){
        return;
    }
    m_fontSize = fontSize;
    repaint();
}

void ScrollPicker::setFontColor(QColor fontColor)
{
    if(m_fontColor == fontColor){
        return;
    }
    m_fontColor = fontColor;
    repaint();
}

void ScrollPicker::setModelColumn(int modelColumn)
{
    if( modelColumn==m_modelColumn ){
        return;
    }
    m_modelColumn = modelColumn;
    update();
}

void ScrollPicker::setHoverColor(QColor hoverColor)
{
    if( hoverColor==m_hoverColor ){
        return;
    }
    m_hoverColor = hoverColor;
    update();
}

void ScrollPicker::setHoverIncreaseFont(int hoverIncreaseFont)
{
    m_hoverIncreaseFont = hoverIncreaseFont;
}

/**
 * @brief 模型中项目数据改变槽函数
 * @param topleft
 * @param bottomright
 * @param roles
 */
void ScrollPicker::modelDataChanged(const QModelIndex &topleft,
                                    const QModelIndex &bottomright,
                                    const QVector<int> &roles)
{
    if( m_currentIndex.row()>=topleft.row() && m_currentIndex.row()<=bottomright.row() && roles.contains(Qt::UserRole) ){
        emit currentTextChanged(currentText());
        update();
    }
}

/**
 * @brief 新项已经插入模型的槽函数，first和last之前都为新项
 * @param parent
 * @param first
 * @param last
 */
void ScrollPicker::modelRowsInserted(const QModelIndex &parent,
                                     int start,
                                     int end)
{
    if(parent!=m_rootIndex){
        return;
    }

    ///如果之前模型为空并当前索引失效设置当前值
    if (start == 0 && (end - start + 1) == count() && !m_currentIndex.isValid()) {
        setCurrentIndex(0,true);
    } else if (m_currentIndex.row() != m_indexBeforeChange) {///如果当前值的索引变化，调用重绘，并emit currentIndexChanged信号
        update();
        emitCurrentIndexChangedSignals();
    }
}

/**
 * @brief 模型中多行已被删除的槽函数
 * @param parent
 * @param first
 * @param last
 */
void ScrollPicker::modelRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first);
    Q_UNUSED(last);

    if(parent!=m_rootIndex){
        return;
    }

    //模型已更改当前索引
    if(m_currentIndex.row()!=m_indexBeforeChange){
        //当前索引失效，模型存在数据
        if( !m_currentIndex.isValid()&&count() ){
            setCurrentIndex(qMin(count() - 1, qMax(m_indexBeforeChange, 0)),true);
            return;
        }
        update();
        emitCurrentIndexChangedSignals();
    }
}

/**
 * @brief 模型被重置的槽函数
 */
void ScrollPicker::modelReset()
{
    if(m_currentIndex.row()!=m_indexBeforeChange){
        emitCurrentIndexChangedSignals();
    }
    update();
}

/**
 * @brief 模型即将新增或删除行的槽函数，保存当前索引的行号
 */
void ScrollPicker::updateIndexBeforeChange()
{
    m_indexBeforeChange = m_currentIndex.row();
}

bool ScrollPicker::event(QEvent *event)
{
    if(event->type()==QEvent::HoverEnter){
        m_hoverd = true;
        update();
    }else if(event->type()==QEvent::HoverLeave){
        m_hoverd = false;
        update();
    }
    return QWidget::event(event);
}

void ScrollPicker::init()
{
    m_animation->setEasingCurve(QEasingCurve::OutQuad);
    m_animation->setDuration(120);

    connect(m_model,&QStandardItemModel::dataChanged,
            this,&ScrollPicker::modelDataChanged);
    connect(m_model,&QStandardItemModel::rowsAboutToBeInserted,
            this,&ScrollPicker::updateIndexBeforeChange);
    connect(m_model,&QStandardItemModel::rowsInserted,
            this,&ScrollPicker::modelRowsInserted);
    connect(m_model,&QStandardItemModel::rowsAboutToBeRemoved,
            this,&ScrollPicker::updateIndexBeforeChange);
    connect(m_model,&QStandardItemModel::rowsRemoved,
            this,&ScrollPicker::modelRowsRemoved);
    connect(m_model,&QStandardItemModel::modelAboutToBeReset,
            this,&ScrollPicker::updateIndexBeforeChange);
    connect(m_model,&QStandardItemModel::modelReset,
            this,&ScrollPicker::modelReset);
}

void ScrollPicker::emitCurrentIndexChangedSignals()
{
    emit currentIndexChanged(currentIndex());
    emit currentTextChanged(currentText());
}

QModelIndex ScrollPicker::getNextModelIndex(const QModelIndex &index)
{
    if(!index.isValid()){
        goto no_next_item;
    }
    ///当前行为最后一行
    if(index.row()==(count()-1)){
        if(!m_loop){
            goto no_next_item;
        }
        return m_model->index(0,m_modelColumn,m_rootIndex);
    }
    return m_model->index(index.row()+1,m_modelColumn,m_rootIndex);
no_next_item:
    return QModelIndex();
}

QModelIndex ScrollPicker::getPrevModelIndex(const QModelIndex &index)
{
    if(!index.isValid()){
        goto no_prev_item;
    }
    ///当前行为第一行
    if(index.row()==0){
        if(!m_loop){
            goto no_prev_item;
        }
        return m_model->index(count()-1,m_modelColumn,m_rootIndex);
    }
    return m_model->index(index.row()-1,m_modelColumn,m_rootIndex);
no_prev_item:
    return QModelIndex();
}

bool ScrollPicker::currentIndexMoveUpper(bool needUpdate)
{
    if(!m_currentIndex.isValid()){
        return false;
    }
    if(m_currentIndex.row()==0){
        if(!m_loop){
            return false;
        }
        setCurrentIndex(count()-1,needUpdate);
    }else{
        setCurrentIndex(m_currentIndex.row()-1,needUpdate);
    }
    return true;
}

bool ScrollPicker::currentIndexMoveDown(bool needUpdate)
{
    if(!m_currentIndex.isValid()){
        return false;
    }
    if(m_currentIndex.row()==(count()-1)){
        if(!m_loop){
            return false;
        }
        setCurrentIndex(0,needUpdate);
    }else{
        setCurrentIndex(m_currentIndex.row()+1,needUpdate);
    }
    return true;
}

void ScrollPicker::homing()
{
    int itemHeight = height()/m_showCount;

    //偏移距离超过一格的一半,改变当前值，重设动画
    if( qAbs(m_deviation) > (itemHeight/2) ){
        if( m_deviation>0 ){
            if(currentIndexMoveUpper(false)){
                m_deviation -= itemHeight;
            }
        }else{
            if(currentIndexMoveDown(false)){
                m_deviation += itemHeight;
            }
        }
    }
    m_animation->setStartValue( m_deviation );
    m_animation->setEndValue(0);
    m_animation->start();
}

void ScrollPicker::clean()
{
    m_model->removeRows(0, m_model->rowCount(m_rootIndex), m_rootIndex);
}

void ScrollPicker::setCurrentIndex(int index,bool needUpdate)
{
    QModelIndex mi = m_model->index(index,m_modelColumn,m_rootIndex);
    bool indexHasChanged = (mi!=m_currentIndex);
    if(indexHasChanged){
        m_currentIndex = QPersistentModelIndex(mi);
        if(needUpdate){
            update();
        }
        emitCurrentIndexChangedSignals();
    }
}

void ScrollPicker::setCurrentText(const QString &text,bool needUpdate)
{
    QModelIndex start = m_model->index(0, m_modelColumn, m_rootIndex);
    const QModelIndexList result = m_model->match(start, Qt::DisplayRole, text, 1,Qt::MatchExactly|Qt::MatchCaseSensitive);
    if (result.isEmpty())
        return;
    QModelIndex firstIndex = result.at(0);
    setCurrentIndex(firstIndex.row(),needUpdate);
}

/**
 * @brief 根据deviation偏移值绘制
 */
void ScrollPicker::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int contentHeight = height();
    int contentWidth = width();
    int itemHeight = contentHeight/showCount();

    auto drawNumber = [=](QPainter& painter,QString text,double deviation,bool isMiddle = false){
        //偏移量越大，字越小
        int fontSize = m_fontSize-m_fontSize*(qAbs(deviation)/contentHeight);
        if(m_hoverd&&isMiddle){
            fontSize += m_hoverIncreaseFont;
        }

        //偏移量越大，越透明
        qreal fontCurveValue = m_fontAlphaEasingCurve.valueForProgress(qAbs(deviation)/(contentHeight/2));
        //透明度限制
        if(fontCurveValue>0.7){
            fontCurveValue = 0.7;
        }
        qreal alphaValue = 255*fontCurveValue;
        int transparency = 255-qFloor(alphaValue);

        //计绘制字体矩形左上的y轴=垂直中心的高度+偏移量-节点一半的高度
        int y = contentHeight/2+deviation-itemHeight/2;

        QFont font("Noto Sans CJK SC Light");
        font.setPointSize(fontSize);
        painter.setFont(font);
        QColor penColor = m_fontColor;
        if(m_hoverd){
            penColor = m_hoverColor;
        }
        penColor.setAlpha(transparency);
        painter.setPen(penColor);

        //计算绘制字体是否有部分在显示中
        QRect drawTextRect(0,y,contentWidth,itemHeight);
        if( this->rect().intersects(drawTextRect) ){
            painter.drawText(drawTextRect,
                             Qt::AlignCenter,
                             text);
        }
    };

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if( !m_currentIndex.isValid() ){
        return QWidget::paintEvent(event);
    }

    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

    ///滑动向下超过一格,更新偏移值，更新按压的y轴距离
    if ( m_deviation >= itemHeight ){
        m_pressedY += itemHeight;
        m_deviation -= itemHeight;

        ///上移当前值
        currentIndexMoveUpper(false);
    }

    ///滑动向上超过一格并且当前值小于最大值，置当前值+1，更新偏移值，更新按压的y轴距离
    if ( m_deviation <= -itemHeight)
    {
        m_pressedY -= itemHeight;
        m_deviation += itemHeight;

        ///下移当前值
        currentIndexMoveDown(false);
    }

    ///绘制当前值
    QVariant displayRoleVar = itemText(m_currentIndex.row());
    drawNumber(painter,displayRoleVar.toString(),m_deviation,true);

    ///绘制上下两侧的值
    QModelIndex upperIndex,downIndex;
    upperIndex = downIndex = m_currentIndex;
    for(int i=1;i<=(m_showCount-1)/2;i++){
        upperIndex = getPrevModelIndex(upperIndex);
        if(upperIndex.isValid()){
            QVariant var = m_model->data(upperIndex);
            drawNumber(painter,var.toString(),m_deviation-(itemHeight*i));
        }
        downIndex = getNextModelIndex(downIndex);
        if(downIndex.isValid()){
            QVariant var = m_model->data(downIndex);
            drawNumber(painter,var.toString(),m_deviation+(itemHeight*i));
        }
    }
}


/**
 * @brief 更改按压状态，记录按压的Y
 */
void ScrollPicker::mousePressEvent(QMouseEvent *event)
{
    if( event->buttons() & Qt::LeftButton ){
        m_animation->stop();
        m_pressed = true;
        m_pressedY = event->localPos().y();
    }
    QWidget::mousePressEvent(event);
}

/**
 * @brief 更改按压状态，判断偏移值是否需要更新当前值,开始动画
 */
void ScrollPicker::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_pressed){
        m_pressed = false;
        m_pressedY = 0;
        homing();
    }
    QWidget::mouseReleaseEvent(event);
}

/**
 * @brief 根据Event的坐标Y和按下坐标的Y计算devitaion偏移量，调用重绘
 */
void ScrollPicker::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_pressed){
        return QWidget::mouseMoveEvent(event);
    }
    if(!m_currentIndex.isValid()){
        return QWidget::mouseMoveEvent(event);;
    }

    int currentY = event->localPos().y();
    int itemHeight = height()/showCount();
    m_deviation = currentY-m_pressedY;

    if( !m_loop && ( (m_deviation<0&&m_currentIndex.row()==(count()-1))||(m_deviation>0&&m_currentIndex.row()==0) ) ){
        m_deviation = 0;
        return QWidget::mouseMoveEvent(event);
    }

    ///避免一次mouseMoveEvent移动距离过大,因为paintEvent中对偏移值判断并对当前值进行增减都是每个间隙来的
    if( qAbs(m_deviation) > itemHeight ){
        m_deviation = m_deviation>0?itemHeight:-itemHeight;
    }

    update();
    return QWidget::mouseMoveEvent(event);
}

/**
 * @brief 根据滚轮事件，更新偏移量，
 */
void ScrollPicker::wheelEvent(QWheelEvent *event)
{
    if(!m_currentIndex.isValid()){
        return QWidget::wheelEvent(event);
    }

    int itemHeight = height()/showCount();
    int delta = event->angleDelta().y();

    if( !m_loop && ( (delta<0&&m_currentIndex.row()==(count()-1))||(delta>0&&m_currentIndex.row()==0) ) ){
        m_deviation = 0;
        return QWidget::wheelEvent(event);
    }

    //滚轮向下
    if(delta<0){
        //切换当前值到下一个
        currentIndexMoveDown(false);
        //偏移量为更新之后的当前值所在的位置
        m_deviation = itemHeight;
    }else{
        currentIndexMoveUpper(false);
        m_deviation = -itemHeight;
    }

    m_animation->setStartValue(m_deviation);
    m_animation->setEndValue(0);
    m_animation->start();
}
