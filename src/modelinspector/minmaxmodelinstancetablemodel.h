#ifndef MINMAXMODELINSTANCETABLEMODEL_H
#define MINMAXMODELINSTANCETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "aggregation.h"
#include "common.h"

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class MinMaxIdentifierFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MinMaxIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
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

class MinMaxModelInstanceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MinMaxModelInstanceTableModel(QObject *parent = nullptr);

    virtual ~MinMaxModelInstanceTableModel();

    void setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance);

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int view() const;

    void setView(int view);

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    int mView;
};

}
}
}

#endif // MINMAXMODELINSTANCETABLEMODEL_H
