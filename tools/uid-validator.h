#ifndef UIDVALIDATOR_H
#define UIDVALIDATOR_H

#include <QValidator>

class UidValidator : public QValidator
{
    Q_OBJECT
public:
    explicit UidValidator(QObject *parent = nullptr);

    virtual State validate(QString &, int &) const Q_DECL_OVERRIDE;
};

#endif  // UIDVALIDATOR_H
