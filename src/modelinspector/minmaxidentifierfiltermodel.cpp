#include "minmaxidentifierfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

MinMaxIdentifierFilterModel::MinMaxIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                         QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void MinMaxIdentifierFilterModel::setIdentifierFilter(const IdentifierFilter &filter,
                                                      const Aggregation &appliedAggregation)
{
    mIdentifierFilter = filter;
    mAppliedAggregation = appliedAggregation;
    invalidateFilter();
}

bool MinMaxIdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
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

bool MinMaxIdentifierFilterModel::filterAcceptsRow(int sourceRow,
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
        Symbol *sym;
        if (mAppliedAggregation.indexToEquation().contains(sectionIndex))
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex);
        else
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex-1);
        if (sym && iter->Text == sym->name()) {
            return false;
        }
    }
    return true;
}



}
}
}
