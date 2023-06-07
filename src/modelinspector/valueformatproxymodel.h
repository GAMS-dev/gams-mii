#ifndef VALUEFORMATPROXYMODEL_H
#define VALUEFORMATPROXYMODEL_H

#include <QIdentityProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ValueFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    ValueFormatProxyModel(QObject *parent = nullptr);

    virtual void setValueFilter(const ValueFilter &valueFilter);

    virtual QVariant data(const QModelIndex &index, int role) const = 0;

protected:
    ValueFilter mValueFilter;
};

class GeneralValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    GeneralValueFormatProxyModel(QObject *parent = nullptr);

    void setValueFilter(const ValueFilter &valueFilter) override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QVariant applyFilter(const QVariant &data) const;

private:
    std::function<double(const QVariant &variant, bool *ok)> getValue;
};

class JaccobianValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    JaccobianValueFormatProxyModel(QObject *parent = nullptr);

    void setValueFilter(const ValueFilter &valueFilter) override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    std::function<double(const QVariant &variant, bool *ok)> getValue;
};

class BPValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    BPValueFormatProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class BPValueFormatTypeProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    BPValueFormatTypeProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class AbsFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    AbsFormatProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    void setAbsFormat(bool absoluteValues);

private:
    bool mAbsoluteValues = false;
};

}
}
}

#endif // VALUEFORMATPROXYMODEL_H
