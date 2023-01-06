#include "aggregationproxymodel.h"
#include "abstractmodelinstance.h"
#include "symbol.h"

#include <QSet>

namespace gams {
namespace studio {
namespace modelinspector {

AggregationProxyModel::AggregationProxyModel(QSharedPointer<AbstractModelInstance> modelInstance,
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
    //mModelInstance->aggregate(mAppliedAggregation); TODO !!! move like MinMax?
    invalidate();
}

QVariant AggregationProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && mAppliedAggregation.isActive()) {
        if (!index.isValid())
            return QVariant();
        QVariant value = mModelInstance->data(index.row(), index.column(), mView);
        return value == 0.0 ? QVariant() : value;
    }
    return QSortFilterProxyModel::data(index, role);
}

QVariant AggregationProxyModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
    if (role == Qt::DisplayRole && mAppliedAggregation.isActive()) {
        auto realIndex = mModelInstance->headerData(section, orientation, mView);
        return realIndex < 0 ? QVariant() : realIndex;
    }
    return QSortFilterProxyModel::headerData(section, orientation, role);
}

int AggregationProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(mAppliedAggregation.viewType());
}

bool AggregationProxyModel::filterAcceptsColumn(int sourceColumn,
                                                const QModelIndex &sourceParent) const
{
    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;
    auto variable = mModelInstance->variable(sectionIndex);
    int startSection = variable->firstSection();
    if (startSection < 0) return true;
    if (mAppliedAggregation.aggregationMap()[Qt::Horizontal].contains(startSection)) {
        return mAppliedAggregation.aggregationMap()[Qt::Horizontal][variable->firstSection()].isSectionVisible(sectionIndex);
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
    int startSection = equation->firstSection();
    if (startSection < 0) return true;
    if (mAppliedAggregation.aggregationMap()[Qt::Vertical].contains(startSection)) {
        return mAppliedAggregation.aggregationMap()[Qt::Vertical][equation->firstSection()].isSectionVisible(sectionIndex);
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

}
}
}
