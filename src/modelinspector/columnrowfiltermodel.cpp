#include "columnrowfiltermodel.h"
#include "symbolmodelinstancetablemodel.h"

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
    bool ok;
    auto entries = sourceModel()->data(sourceModel()->index(0, sourceColumn, sourceParent),
                                       SymbolModelInstanceTableModel::ColumnEntryRole).toInt(&ok);
    return ok && entries;
}

bool ColumnRowFilterModel::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    bool ok;
    auto entries = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent),
                                       SymbolModelInstanceTableModel::RowEntryRole).toInt(&ok);
    return ok && entries;
}

}
}
}
