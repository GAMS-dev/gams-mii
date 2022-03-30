#ifndef LABELFILTERMODEL_H
#define LABELFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance;

class LabelFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    LabelFilterModel(QSharedPointer<ModelInstance> modelInstance,
                     QObject *parent = nullptr);

    LabelFilter labelFilter() const;
    void setLabelFilter(const LabelFilter &filter);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;

private:
    bool matchesAllColumnLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                int sectionIndex) const;
    bool matchesAnyColumnLabels(const QMap<QString, Qt::CheckState> &checkStates,
                                int sectionIndex) const;

    bool matchesAllRowLabels(const QMap<QString, Qt::CheckState> &checkStates,
                             int sectionIndex) const;
    bool matchesAnyRowLabels(const QMap<QString, Qt::CheckState> &checkStates,
                             int sectionIndex) const;

private:
    QSharedPointer<ModelInstance> mModelInstance;
    LabelFilter mLabelFilter;
};

}
}
}

#endif // LABELFILTERMODEL_H
