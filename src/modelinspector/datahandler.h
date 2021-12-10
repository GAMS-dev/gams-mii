#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "common.h"
#include "aggregation.h"

#include <QVariant>

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

class DataHandler
{
public:
    class AbstractDataAggregator;

    DataHandler();

    const Aggregation& appliedAggregation() const;
    void setAppliedAggregation(const Aggregation &aggregation);

    void aggregate(ModelInstance *modelInstance);

    QVariant data(int row, int column) const;
    void loadDataMatrix(ModelInstance *modelInstance);

    bool isAggregationActive() const;

private:
    void setDataAggregator(Aggregation::Type type);

    void applyValueFilter();

    void applyRowFilter(ModelInstance *modelInstance);
    void applyColumnFilter(ModelInstance *modelInstance);

    void setDefaultColumnValues(int columnCount);

    IndexCheckState checkStates(Qt::Orientation orientation,
                                ModelInstance *modelInstance) const;

private:
    QSharedPointer<AbstractDataAggregator> mDataAggregator;
    Aggregation mAppliedAggregation;

    DataMatrix mDataMatrix;
    DataMatrix mAggrMatrix;
};

}
}
}

#endif // DATAHANDLER_H
