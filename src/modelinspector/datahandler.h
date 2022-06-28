#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "aggregation.h"

#include <QVariant>

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

typedef int View;
typedef QMap<Qt::Orientation, QList<int>> SectionMapping;

class DataHandler
{
public:
    class AbstractDataAggregator;

    DataHandler();

    void aggregate(const Aggregation &aggregation, ModelInstance *modelInstance);

    QVariant data(int row, int column, int view) const;

    int headerData(int logicalIndex, Qt::Orientation orientation, int view) const;

    void loadDataMatrix(ModelInstance *modelInstance);

private:
    void setDataAggregator(const Aggregation &appliedAggregation);

    void applyValueFilter();

    void applyRowFilter(ModelInstance *modelInstance);
    void applyColumnFilter(ModelInstance *modelInstance);

    void setDefaultColumnValues(int columnCount);

    IndexCheckStates checkStates(Qt::Orientation orientation,
                                 ModelInstance *modelInstance) const;

    bool keepColumn(int column);
    bool keepRow(int row);

private:
    QSharedPointer<AbstractDataAggregator> mDataAggregator;

    DataMatrix mDataMatrix;
    QMap<View, DataMatrix> mAggrCache;
    QMap<View, SectionMapping> mLogicalSectionMapping;
};

}
}
}

#endif // DATAHANDLER_H
