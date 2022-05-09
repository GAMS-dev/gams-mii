#ifndef IDENTIFIERFILTERMODEL_H
#define IDENTIFIERFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"
#include "symbolinfo.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

class IdentifierFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    IdentifierFilterModel(QSharedPointer<ModelInstance> modelInstance,
                          QObject *parent = nullptr);

    IdentifierFilter& identifierFilter();
    void setIdentifierFilter(const IdentifierFilter &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    SymbolInfo symbol(DataSource dataSource, int sectionIndex) const;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
};

class IdentifierLabelFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    IdentifierLabelFilterModel(QSharedPointer<ModelInstance> modelInstance,
                               QObject *parent = nullptr);

    void clearIdentifierFilter();

    IdentifierFilter& identifierFilter();
    IdentifierState identifierState(int symbolIndex,
                                    Qt::Orientation orientation) const;
    void setIdentifierState(const IdentifierState &state,
                            Qt::Orientation orientation);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    int startSection(DataSource dataSource, int sectionIndex) const;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    IdentifierFilter mIdentifierFilter;
};

}
}
}

#endif // IDENTIFIERFILTERMODEL_H
