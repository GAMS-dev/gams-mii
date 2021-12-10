#include "identifierfiltermodel.h"
#include "modelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

IdentifierFilterModel::IdentifierFilterModel(QSharedPointer<ModelInstance> modelInstance,
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

void IdentifierFilterModel::setIdentifierCheckState(int symbolIndex, Qt::CheckState state,
                                            Qt::Orientation orientation)
{
    auto symbols = mIdentifierFilter[orientation];
    for (auto iter=symbols.begin(); iter!=symbols.end(); ++iter) {
        if (iter->SymbolIndex == symbolIndex) {
            iter->Checked = state;
            break;
        }
    }
    mIdentifierFilter[orientation] = symbols;
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
        if (sectionIndex < PredefinedHeaderLength &&
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
        if (sectionIndex < PredefinedHeaderLength &&
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

IdentifierLabelFilterModel::IdentifierLabelFilterModel(QSharedPointer<ModelInstance> modelInstance,
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

    int startSection = mModelInstance->variable(sectionIndex).firstSection();
    if (startSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Horizontal][startSection];
    for (auto iter=item.LabelCheckStates.keyValueBegin(); iter!=item.LabelCheckStates.keyValueEnd(); ++iter) {
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

    int startSection = mModelInstance->equation(sectionIndex).firstSection();
    if (startSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Vertical][startSection];
    for (auto iter=item.LabelCheckStates.keyValueBegin(); iter!=item.LabelCheckStates.keyValueEnd(); ++iter) {
        if (sectionIndex == iter->first && iter->second == Qt::Unchecked) {
            return false;
        }
    }
    return true;
}

}
}
}
