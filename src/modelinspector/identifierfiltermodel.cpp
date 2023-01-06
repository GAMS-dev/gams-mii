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

    for (auto iter=mIdentifierFilter[Qt::Horizontal].constBegin();
            iter!=mIdentifierFilter[Qt::Horizontal].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        if (iter->Text == sym->name() && sectionIndex >= sym->firstSection() &&
                sectionIndex <= sym->lastSection()) {
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

    for (auto iter=mIdentifierFilter[Qt::Vertical].constBegin();
            iter!=mIdentifierFilter[Qt::Vertical].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto sym = mModelInstance->equation(sectionIndex);
        if (iter->Text == sym->name() && sectionIndex >= sym->firstSection() &&
                sectionIndex <= sym->lastSection()) {
            return false;
        }
    }
    return true;
}

VarIdentifierFilterModel::VarIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                   QObject *parent)
    : IdentifierFilterModel(modelInstance, parent)
{

}

bool VarIdentifierFilterModel::filterAcceptsRow(int sourceRow,
                                                     const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    if (mIdentifierFilter[Qt::Vertical].isEmpty())
        return true;
    auto text = sourceModel()->headerData(sourceRow, Qt::Vertical).toString();
    auto sectionIndex = constant->PredefinedHeader.indexOf(text);
    auto item = mIdentifierFilter[Qt::Vertical].value(sectionIndex);
    return item.Checked == Qt::Unchecked ? false : true;
}

EqnIdentifierFilterModel::EqnIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                   QObject *parent)
    : IdentifierFilterModel(modelInstance, parent)
{

}

bool EqnIdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
                                                   const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    if (mIdentifierFilter[Qt::Horizontal].isEmpty())
        return true;
    auto text = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toString();
    auto sectionIndex = constant->PredefinedHeader.indexOf(text);
    auto item = mIdentifierFilter[Qt::Horizontal].value(sectionIndex);
    return item.Checked == Qt::Unchecked ? false : true;
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

IdentifierState IdentifierLabelFilterModel::identifierState(int symbolIndex,
                                                            Qt::Orientation orientation) const
{
    return mIdentifierFilter[orientation].value(symbolIndex);
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
    int firstSection = mModelInstance->variable(sectionIndex)->firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Horizontal].value(firstSection);
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
    int firstSection = mModelInstance->equation(sectionIndex)->firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Vertical].value(firstSection);
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
