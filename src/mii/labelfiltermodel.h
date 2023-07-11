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
#ifndef LABELFILTERMODEL_H
#define LABELFILTERMODEL_H

#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace mii {

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

}
}
}

#endif // LABELFILTERMODEL_H
