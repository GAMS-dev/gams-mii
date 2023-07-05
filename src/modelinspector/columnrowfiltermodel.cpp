/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "columnrowfiltermodel.h"
#include "common.h"

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
                                       Mi::ColumnEntryRole).toInt(&ok);
    return ok && entries;
}

bool ColumnRowFilterModel::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    bool ok;
    auto entries = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent),
                                       Mi::RowEntryRole).toInt(&ok);
    return ok && entries;
}

}
}
}
