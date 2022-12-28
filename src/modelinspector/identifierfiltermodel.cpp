#include "identifierfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

IdentifierFilterModel::IdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                             QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

IdentifierFilter& IdentifierFilterModel::identifierFilter()
{
    return mIdentifierFilter;
}

void IdentifierFilterModel::setIdentifierFilter(const IdentifierFilter &filter)
{
    mIdentifierFilter = filter;
    invalidateFilter();
}

bool IdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
                                                const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    Q_FOREACH(const auto& item, mIdentifierFilter[Qt::Horizontal]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sectionIndex < constant->PredefinedHeaderLength &&
                item.SymbolIndex == sectionIndex) {
            return false;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        if (item.Text == sym.name() && sectionIndex >= sym.firstSection() &&
                sectionIndex <= sym.lastSection()) {
            return false;
        }
    }
    return true;
}

bool IdentifierFilterModel::filterAcceptsRow(int sourceRow,
                                             const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    Q_FOREACH(const auto& item, mIdentifierFilter[Qt::Vertical]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sectionIndex < constant->PredefinedHeaderLength &&
                item.SymbolIndex == sectionIndex) {
            return false;
        }
        auto sym = mModelInstance->equation(sectionIndex);
        if (item.Text == sym.name() && sectionIndex >= sym.firstSection() &&
                sectionIndex <= sym.lastSection()) {
            return false;
        }
    }
    return true;
}

IdentifierLabelFilterModel::IdentifierLabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                       QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void IdentifierLabelFilterModel::clearIdentifierFilter()
{
    mIdentifierFilter.clear();
}

IdentifierFilter& IdentifierLabelFilterModel::identifierFilter()
{
    return mIdentifierFilter;
}

IdentifierState IdentifierLabelFilterModel::identifierState(int symbolIndex,
                                                            Qt::Orientation orientation) const
{
    return mIdentifierFilter[orientation][symbolIndex];
}

void IdentifierLabelFilterModel::setIdentifierState(const IdentifierState &state,
                                                    Qt::Orientation orientation)
{
    mIdentifierFilter[orientation][state.SymbolIndex] = state;
    invalidateFilter();
}

bool IdentifierLabelFilterModel::filterAcceptsColumn(int sourceColumn,
                                                     const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    if (mIdentifierFilter[Qt::Horizontal].isEmpty())
        return true;
    int firstSection = mModelInstance->variable(sectionIndex).firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Horizontal][firstSection];
    for (auto iter=item.CheckStates.keyValueBegin(); iter!=item.CheckStates.keyValueEnd(); ++iter) {
        if (sectionIndex == iter->first && iter->second == Qt::Unchecked) {
            return false;
        }
    }
    return true;
}

bool IdentifierLabelFilterModel::filterAcceptsRow(int sourceRow,
                                                  const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    if (mIdentifierFilter[Qt::Vertical].isEmpty())
        return true;
    int firstSection = mModelInstance->equation(sectionIndex).firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Vertical][firstSection];
    for (auto iter=item.CheckStates.keyValueBegin(); iter!=item.CheckStates.keyValueEnd(); ++iter) {
        if (sectionIndex == iter->first && iter->second == Qt::Unchecked) {
            return false;
        }
    }
    return true;
}

}
}
}
