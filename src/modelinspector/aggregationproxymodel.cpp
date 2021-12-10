#include "aggregationproxymodel.h"
#include "modelinstance.h"
#include "symbolinfo.h"

namespace gams {
namespace studio {
namespace modelinspector {

AggregationProxyModel::AggregationProxyModel(QSharedPointer<ModelInstance> modelInstance,
                                                             QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

const Aggregation& AggregationProxyModel::aggregation() const
{
    return mAggregation;
}

const Aggregation& AggregationProxyModel::appliedAggregation() const
{
    return mAppliedAggregation;
}

void AggregationProxyModel::setAggregation(const Aggregation &aggregation,
                                           const Aggregation &appliedAggregation)
{
    mAggregation = aggregation;
    mAppliedAggregation = appliedAggregation;
    mModelInstance->setAppliedAggregation(mAppliedAggregation);
    invalidate();
}

QVariant AggregationProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && mModelInstance->isAggregationActive()) {
        if (!index.isValid())
            return QVariant();
        auto value = mModelInstance->data(index.row(), index.column());
        return value == 0.0 ? QVariant() : value;
    }
    return QSortFilterProxyModel::data(index, role);
}

bool AggregationProxyModel::filterAcceptsColumn(int sourceColumn,
                                                const QModelIndex &sourceParent) const
{
    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;
    auto variable = mModelInstance->variable(sectionIndex);
    int startSection = variable.firstSection();
    if (startSection < 0)
        return true;
    if (mAppliedAggregation.aggregationMap()[Qt::Horizontal].contains(startSection)) {
        return mAppliedAggregation.aggregationMap()[Qt::Horizontal][variable.firstSection()].isSectionVisible(sectionIndex);
    }
    return QSortFilterProxyModel::filterAcceptsColumn(sourceColumn, sourceParent);
}

bool AggregationProxyModel::filterAcceptsRow(int sourceRow,
                                             const QModelIndex &sourceParent) const
{
    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;
    auto equation = mModelInstance->equation(sectionIndex);
    int startSection = equation.firstSection();
    if (startSection < 0)
        return true;
    if (mAppliedAggregation.aggregationMap()[Qt::Vertical].contains(startSection)) {
        return mAppliedAggregation.aggregationMap()[Qt::Vertical][equation.firstSection()].isSectionVisible(sectionIndex);
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

}
}
}
