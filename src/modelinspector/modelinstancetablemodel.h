#ifndef MODELINSTANCETABLEMODEL_H
#define MODELINSTANCETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

#include "aggregation.h"
#include "common.h"

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class ModelInstanceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelInstanceTableModel(QObject *parent = nullptr);

    ~ModelInstanceTableModel();

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

private:
    PredefinedViewEnum mViewType;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    int mView;
};

class MinMaxModelInstanceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MinMaxModelInstanceTableModel(QObject *parent = nullptr);

    ~MinMaxModelInstanceTableModel();

    const Aggregation& aggregation() const;
    const Aggregation& appliedAggregation() const;

    void setAggregation(const Aggregation &aggregation,
                        const Aggregation &appliedAggregation);

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

private:
    PredefinedViewEnum mViewType;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    Aggregation mAggregation;
    Aggregation mAppliedAggregation;
    int mView;
};

}
}
}

#endif // MODELINSTANCETABLEMODEL_H
