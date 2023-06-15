#include "valueformatproxymodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

ValueFormatProxyModel::ValueFormatProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{

}

void ValueFormatProxyModel::setValueFilter(const ValueFilter &valueFilter)
{
    beginResetModel();
    mValueFilter = valueFilter;
    endResetModel();
}

GeneralValueFormatProxyModel::GeneralValueFormatProxyModel(QObject *parent)
    : ValueFormatProxyModel(parent)
{
    getValue = [](const QVariant &variant, bool *ok) {
        Q_UNUSED(variant);
        return variant.toDouble(ok);
    };
}

void GeneralValueFormatProxyModel::setValueFilter(const ValueFilter &valueFilter)
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

QVariant GeneralValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    return applyFilter(QIdentityProxyModel::data(index, role));
}

QVariant GeneralValueFormatProxyModel::applyFilter(const QVariant &data) const
{
    if (!data.toString().compare(Mi::SV_EPS, Qt::CaseInsensitive)) {
        if  (mValueFilter.ShowEps)
            return data;
        return QVariant();
    }
    if (!data.toString().compare(Mi::SV_PINF, Qt::CaseInsensitive)) {
        if (mValueFilter.ShowPInf)
            return data;
        return QVariant();
    }
    if (!data.toString().compare(Mi::SV_NINF, Qt::CaseInsensitive)) {
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

JaccobianValueFormatProxyModel::JaccobianValueFormatProxyModel(QObject *parent)
    : ValueFormatProxyModel(parent)
{
    getValue = [](const QVariant &variant, bool *ok) {
        Q_UNUSED(variant);
        return variant.toDouble(ok);
    };
}

void JaccobianValueFormatProxyModel::setValueFilter(const ValueFilter &valueFilter)
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

QVariant JaccobianValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return sourceModel()->data(index, role);
    bool ok;
    double value = getValue(QIdentityProxyModel::data(index, role), &ok);
    if (ok) {
        if (!mValueFilter.ExcludeRange && value >= mValueFilter.MinValue && value <= mValueFilter.MaxValue)
            return value;
        else if (mValueFilter.ExcludeRange && (value < mValueFilter.MinValue || value > mValueFilter.MaxValue))
            return value;
    }
    return QVariant();
}

BPValueFormatProxyModel::BPValueFormatProxyModel(QObject *parent)
    : ValueFormatProxyModel(parent)
{

}

QVariant BPValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    bool ok;
    double value = QIdentityProxyModel::data(index, role).toDouble(&ok);
    if (ok) {
        double retval = value;
        if (mValueFilter.UseAbsoluteValues)
            value = std::abs(value);
        if (!mValueFilter.ExcludeRange && value >= mValueFilter.MinValue && value <= mValueFilter.MaxValue)
            return retval;
        else if (mValueFilter.ExcludeRange && (value < mValueFilter.MinValue || value > mValueFilter.MaxValue))
            return retval;
    }
    return QVariant();
}

BPValueFormatTypeProxyModel::BPValueFormatTypeProxyModel(QObject *parent)
    : ValueFormatProxyModel(parent)
{

}

QVariant BPValueFormatTypeProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole ||
        index.column() == columnCount()-4 || index.row() == rowCount()-1)
        return QIdentityProxyModel::data(index, role);
    bool ok;
    double value = QIdentityProxyModel::data(index, role).toDouble(&ok);
    if (ok) {
        double retval = value;
        if (mValueFilter.UseAbsoluteValues)
            value = std::abs(value);
        if (!mValueFilter.ExcludeRange && value >= mValueFilter.MinValue && value <= mValueFilter.MaxValue)
            return retval;
        else if (mValueFilter.ExcludeRange && (value < mValueFilter.MinValue || value > mValueFilter.MaxValue))
            return retval;
    }
    return QVariant();
}

AbsFormatProxyModel::AbsFormatProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{

}

QVariant AbsFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole) {
        bool ok;
        double value = QIdentityProxyModel::data(index, role).toDouble(&ok);
        if (ok && mAbsoluteValues)
            return std::abs(value);
    }
    return QIdentityProxyModel::data(index, role);
}

void AbsFormatProxyModel::setAbsFormat(bool absoluteValues)
{
    beginResetModel();
    mAbsoluteValues = absoluteValues;
    endResetModel();
}

}
}
}
