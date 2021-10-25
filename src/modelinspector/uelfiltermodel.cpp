#include "uelfiltermodel.h"
#include "modelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

UelFilterModel::UelFilterModel(QSharedPointer<ModelInstance> modelInstance,
                               QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

UelFilterMap UelFilterModel::uelFilter() const
{
    return mUelFilter;
}

void UelFilterModel::setUelFilter(const UelFilterMap &filter)
{
    mUelFilter = filter;
    invalidateFilter();
}

bool UelFilterModel::filterAcceptsColumn(int sourceColumn,
                                         const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    UelFilter filter = mUelFilter[Qt::Horizontal];
    for (auto iter=filter.constKeyValueBegin(); iter!=filter.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked || sourceColumn < PredefinedHeaderLength) {
            continue;
        }
        auto sym = mModelInstance->variable(sourceColumn);
        auto uels = sym.DimensionData[sourceColumn];
        if (uels.contains(iter->first)) {
            return false;
        }
    }
    return true;
}

bool UelFilterModel::filterAcceptsRow(int sourceRow,
                                      const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    UelFilter filter = mUelFilter[Qt::Vertical];
    for (auto iter=filter.constKeyValueBegin(); iter!=filter.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked || sourceRow < PredefinedHeaderLength) {
            continue;
        }
        auto sym = mModelInstance->equation(sourceRow);
        auto uels = sym.DimensionData[sourceRow];
        if (uels.contains(iter->first)) {
            return false;
        }
    }
    return true;
}

}
}
}
