#ifndef LABELFILTERMODEL_H
#define LABELFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;

class LabelFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    LabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                     QObject *parent = nullptr);

    LabelFilter labelFilter() const;
    void setLabelFilter(const LabelFilter &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

    bool matchesAllColumnLabels(const LabelCheckStates &checkStates,
                                int sectionIndex) const;
    bool matchesAnyColumnLabels(const LabelCheckStates &checkStates,
                                int sectionIndex) const;

    bool matchesAllRowLabels(const LabelCheckStates &checkStates,
                             int sectionIndex) const;
    bool matchesAnyRowLabels(const LabelCheckStates &checkStates,
                             int sectionIndex) const;

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    LabelFilter mLabelFilter;
};

class EqnLabelFilterModel : public LabelFilterModel
{
    Q_OBJECT

public:
    EqnLabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                        QObject *parent = nullptr);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;
};

class VarLabelFilterModel : public LabelFilterModel
{
    Q_OBJECT

public:
    VarLabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                        QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
};

}
}
}

#endif // LABELFILTERMODEL_H
