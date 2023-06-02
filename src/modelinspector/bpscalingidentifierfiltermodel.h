#ifndef BPSCALINGIDENTIFIERFILTERMODEL_H
#define BPSCALINGIDENTIFIERFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "aggregation.h"
#include "common.h"

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class BPScalingIdentifierFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    BPScalingIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                QObject *parent = nullptr);

    void setIdentifierFilter(const IdentifierFilter &filter, const Aggregation &appliedAggregation);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
    Aggregation mAppliedAggregation;
};

}
}
}

#endif // BPSCALINGIDENTIFIERFILTERMODEL_H
