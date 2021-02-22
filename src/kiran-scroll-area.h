#ifndef KIRANSCROLLAREA_H
#define KIRANSCROLLAREA_H

#include <QScrollArea>

class KiranScrollArea : public QScrollArea
{
public:
    KiranScrollArea(QWidget* parent=nullptr);
    ~KiranScrollArea()=default;

    virtual QSize sizeHint() const override;
};

#endif // KIRANSCROLLAREA_H
