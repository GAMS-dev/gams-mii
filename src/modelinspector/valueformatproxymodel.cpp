#include "valueformatproxymodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

ValueFormatProxyModel::ValueFormatProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{

}

ValueFilter ValueFormatProxyModel::valueFilter() const
{
    return mValueFilter;
}

void ValueFormatProxyModel::setValueFilter(const ValueFilter &valueFilter)
{
    beginResetModel();
    mValueFilter = valueFilter;
    endResetModel();
}

QVariant ValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    return applyFilter(sourceModel()->data(index, role));
}

QVariant ValueFormatProxyModel::applyFilter(const QVariant &data) const
{
    if (!data.toString().compare("eps", Qt::CaseInsensitive)) {
        if  (mValueFilter.ShowEps)
            return data;
        return QVariant();
    }
    if (!data.toString().compare("+inf", Qt::CaseInsensitive)) {
        if (mValueFilter.ShowPInf)
            return data;
        return QVariant();
    }
    if (!data.toString().compare("-inf", Qt::CaseInsensitive)) {
        if (mValueFilter.ShowNInf)
            return data;
        return QVariant();
    }
    bool ok;
    double value = data.toDouble(&ok);
    if (ok) {
        if (!mValueFilter.Exclude && value >= mValueFilter.MinValue && value <= mValueFilter.MaxValue)
            return data;
        else if (mValueFilter.Exclude && (value < mValueFilter.MinValue || value > mValueFilter.MaxValue))
            return data;
    }
    return QVariant();
}

}
}
}
