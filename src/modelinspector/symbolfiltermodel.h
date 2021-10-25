#ifndef SYMBOLFILTERMODEL_H
#define SYMBOLFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

class SymbolFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SymbolFilterModel(QSharedPointer<ModelInstance> modelInstance,
                      QObject *parent = nullptr);

    SymbolFilterMap& symbolFilter();
    void setSymbolFilter(const SymbolFilterMap &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    SymbolFilterMap mSymbolFilter;
};

}
}
}

#endif // SYMBOLFILTERMODEL_H
