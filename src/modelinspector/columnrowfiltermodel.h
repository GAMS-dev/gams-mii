#ifndef COLUMNROWFILTERMODEL_H
#define COLUMNROWFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace gams {
namespace studio {
namespace modelinspector {

class ColumnRowFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ColumnRowFilterModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsColumn(int sourceColumn,
                             const QModelIndex &sourceParent) const override;

    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
};

}
}
}

#endif // COLUMNROWFILTERMODEL_H
