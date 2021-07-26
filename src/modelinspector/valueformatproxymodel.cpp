#include "valueformatproxymodel.h"
#include "hierarchicalheaderview.h"

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

ValueFormatProxyModel::ValueFormatProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{

}

void ValueFormatProxyModel::setSettings(const ValueFilterSettings &settings)
{
    beginResetModel();
    mSettings = settings;
    endResetModel();
}

QVariant ValueFormatProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == HierarchicalHeaderView::HorizontalHeaderDataRole ||
        role == HierarchicalHeaderView::VerticalHeaderDataRole) {
        return sourceModel()->data(index, role);
    }
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QIdentityProxyModel::data(index, role);
    return applyFilter(sourceModel()->data(index, role));
}

QVariant ValueFormatProxyModel::applyFilter(const QVariant &data) const
{
    if (!data.toString().compare("eps", Qt::CaseInsensitive)) {
        if  (mSettings.ShowEps)
            return data;
        return QVariant();
    }
    if (!data.toString().compare("+inf", Qt::CaseInsensitive)) {
        if (mSettings.ShowPInf)
            return data;
        return QVariant();
    }
    if (!data.toString().compare("-inf", Qt::CaseInsensitive)) {
        if (mSettings.ShowNInf)
            return data;
        return QVariant();
    }
    bool ok;
    double value = data.toDouble(&ok);
    if (ok) {
        if (!mSettings.Exclude && value >= mSettings.MinValue && value <= mSettings.MaxValue)
            return data;
        else if (mSettings.Exclude && (value < mSettings.MinValue || value > mSettings.MaxValue))
            return data;
    }
    return QVariant();
}

}
}
}
