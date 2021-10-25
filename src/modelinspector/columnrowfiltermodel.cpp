#include "columnrowfiltermodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

ColumnRowFilterModel::ColumnRowFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool ColumnRowFilterModel::filterAcceptsColumn(int sourceColumn,
                                               const QModelIndex &sourceParent) const
{
    for (int r=0; r<sourceModel()->rowCount(); ++r) {
        if (!sourceModel()->index(r, sourceColumn, sourceParent).data().toString().isEmpty())
            return true;
    }
    return false;
}

bool ColumnRowFilterModel::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    for (int c=0; c<sourceModel()->columnCount(); ++c) {
        if (!sourceModel()->index(sourceRow, c, sourceParent).data().toString().isEmpty())
            return true;
    }
    return false;
}

}
}
}
