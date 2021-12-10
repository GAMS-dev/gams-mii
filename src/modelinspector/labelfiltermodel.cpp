#include "labelfiltermodel.h"
#include "modelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

LabelFilterModel::LabelFilterModel(QSharedPointer<ModelInstance> modelInstance,
                                   QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

LabelFilter LabelFilterModel::labelFilter() const
{
    return mLabelFilter;
}

void LabelFilterModel::setLabelFilter(const LabelFilter &filter)
{
    mLabelFilter = filter;
    invalidateFilter();
}

bool LabelFilterModel::filterAcceptsColumn(int sourceColumn,
                                           const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    LabelStates filter = mLabelFilter[Qt::Horizontal];
    for (auto iter=filter.constKeyValueBegin(); iter!=filter.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked || sectionIndex < PredefinedHeaderLength) {
            continue;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        auto labels = sym.sectionLabels()[sectionIndex];
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool LabelFilterModel::filterAcceptsRow(int sourceRow,
                                        const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    LabelStates filter = mLabelFilter[Qt::Vertical];
    for (auto iter=filter.constKeyValueBegin(); iter!=filter.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked || sectionIndex < PredefinedHeaderLength) {
            continue;
        }
        auto sym = mModelInstance->equation(sectionIndex);
        auto labels = sym.sectionLabels()[sectionIndex];
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

}
}
}
