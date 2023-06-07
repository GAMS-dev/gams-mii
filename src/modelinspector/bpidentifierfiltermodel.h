#ifndef BPIDENTIFIERFILTERMODEL_H
#define BPIDENTIFIERFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class BPIdentifierFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    BPIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                            QObject *parent = nullptr);

    void setIdentifierFilter(const IdentifierFilter &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
};

}
}
}

#endif // BPIDENTIFIERFILTERMODEL_H
