#include "valueformatproxymodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

ValueFormatProxyModel::ValueFormatProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
    getValue = [](const QVariant &variant, bool *ok) {
        Q_UNUSED(variant);
        return variant.toDouble(ok);
    };
}

ValueFilter ValueFormatProxyModel::valueFilter() const
{
    return mValueFilter;
}

void ValueFormatProxyModel::setValueFilter(const ValueFilter &valueFilter)
{
    beginResetModel();
    mValueFilter = valueFilter;
    if (mValueFilter.UseAbsoluteValues) {
        getValue = [](const QVariant &variant, bool *ok) {
            return std::abs(variant.toDouble(ok));
        };
    } else {
        getValue = [](const QVariant &variant, bool *ok) {
            return variant.toDouble(ok);
        };
    }
    endResetModel();
}

QVariant ValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    return applyFilter(QIdentityProxyModel::data(index, role));
}

QVariant ValueFormatProxyModel::applyFilter(const QVariant &data) const
{
    if (!data.toString().compare(constant->EPS, Qt::CaseInsensitive)) {
        if  (mValueFilter.ShowEps)
            return data;
        return QVariant();
    }
    if (!data.toString().compare(constant->P_INF, Qt::CaseInsensitive)) {
        if (mValueFilter.ShowPInf)
            return data;
        return QVariant();
    }
    if (!data.toString().compare(constant->N_INF, Qt::CaseInsensitive)) {
        if (mValueFilter.ShowNInf)
            return data;
        return QVariant();
    }
    bool ok;
    double value = getValue(data, &ok);
    if (ok) {
        if (!mValueFilter.ExcludeRange && value >= mValueFilter.MinValue && value <= mValueFilter.MaxValue)
            return value;
        else if (mValueFilter.ExcludeRange && (value < mValueFilter.MinValue || value > mValueFilter.MaxValue))
            return value;
    }
    return QVariant();
}

}
}
}
