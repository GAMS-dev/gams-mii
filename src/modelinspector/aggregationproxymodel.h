#ifndef AGGREGATIONPROXYMODEL_H
#define AGGREGATIONPROXYMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"
#include "aggregation.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;
class SymbolInfo;

class AggregationProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AggregationProxyModel(QSharedPointer<ModelInstance> modelInstance,
                          QObject *parent = nullptr);

    const Aggregation& aggregation() const;
    const Aggregation& appliedAggregation() const;
    void setAggregation(const Aggregation &aggregation,
                        const Aggregation &appliedAggregation);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    Aggregation mAggregation;
    Aggregation mAppliedAggregation;

    bool mFullyAggregated;
};

}
}
}

#endif // AGGREGATIONPROXYMODEL_H
