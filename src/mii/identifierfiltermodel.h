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
#ifndef IDENTIFIERFILTERMODEL_H
#define IDENTIFIERFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace mii {

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
