#ifndef UELFILTERMODEL_H
#define UELFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

class UelFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    UelFilterModel(QSharedPointer<ModelInstance> modelInstance,
                   QObject *parent = nullptr);

    UelFilterMap uelFilter() const;
    void setUelFilter(const UelFilterMap &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    UelFilterMap mUelFilter;
};

}
}
}

#endif // UELFILTERMODEL_H
