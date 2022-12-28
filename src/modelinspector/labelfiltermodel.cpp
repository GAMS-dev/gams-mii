#include "labelfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

LabelFilterModel::LabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
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
    if (!ok || sectionIndex < constant->PredefinedHeaderLength) return true;

    auto filter = mLabelFilter.LabelCheckStates[Qt::Horizontal];
    if (mLabelFilter.Any)
        return matchesAnyColumnLabels(filter, sectionIndex);
    return matchesAllColumnLabels(filter, sectionIndex);
}

bool LabelFilterModel::filterAcceptsRow(int sourceRow,
                                        const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok || sectionIndex < constant->PredefinedHeaderLength) return true;

    auto filter = mLabelFilter.LabelCheckStates[Qt::Vertical];
    if (mLabelFilter.Any)
        return matchesAnyRowLabels(filter, sectionIndex);
    return matchesAllRowLabels(filter, sectionIndex);
}

bool LabelFilterModel::matchesAllColumnLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                              int sectionIndex) const
{
    auto sym = mModelInstance->variable(sectionIndex);
    auto labels = sym.sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked) {
            continue;
        }
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool LabelFilterModel::matchesAnyColumnLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                              int sectionIndex) const
{
    auto sym = mModelInstance->variable(sectionIndex);
    if (sym.isScalar()) return true;
    auto labels = sym.sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked && labels.contains(iter->first, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool LabelFilterModel::matchesAllRowLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                           int sectionIndex) const
{
    auto sym = mModelInstance->equation(sectionIndex);
    auto labels = sym.sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked) {
            continue;
        }
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool LabelFilterModel::matchesAnyRowLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                           int sectionIndex) const
{
    auto sym = mModelInstance->equation(sectionIndex);
    if (sym.isScalar()) return true;
    auto labels = sym.sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked && labels.contains(iter->first, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

}
}
}
