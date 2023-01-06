#ifndef IDENTIFIERFILTERMODEL_H
#define IDENTIFIERFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;

class IdentifierFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    IdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                          QObject *parent = nullptr);

    void setIdentifierFilter(const IdentifierFilter &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
};

class VarIdentifierFilterModel : public IdentifierFilterModel
{
    Q_OBJECT

public:
    VarIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                          QObject *parent = nullptr);

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
};

class EqnIdentifierFilterModel : public IdentifierFilterModel
{
    Q_OBJECT

public:
    EqnIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                          QObject *parent = nullptr);

    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;
};

class IdentifierLabelFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    IdentifierLabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                               QObject *parent = nullptr);

    void clearIdentifierFilter();

    IdentifierState identifierState(int symbolIndex,
                                    Qt::Orientation orientation) const;
    void setIdentifierState(const IdentifierState &state,
                            Qt::Orientation orientation);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
};

}
}
}

#endif // IDENTIFIERFILTERMODEL_H
