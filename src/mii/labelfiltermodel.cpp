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
#include "labelfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio {
namespace mii {

LabelFilterModel::LabelFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                   QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

LabelFilter LabelFilterModel::labelFilter() const
{
    return mLabelFilter;
}

void LabelFilterModel::setLabelFilter(const LabelFilter &filter)
{
    mLabelFilter = filter;
    invalidateFilter();
}

bool LabelFilterModel::filterAcceptsColumn(int sourceColumn,
                                           const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    auto filter = mLabelFilter.LabelCheckStates[Qt::Horizontal];
    if (mLabelFilter.Any)
        return matchesAnyColumnLabels(filter, sectionIndex);
    return matchesAllColumnLabels(filter, sectionIndex);
}

bool LabelFilterModel::filterAcceptsRow(int sourceRow,
                                        const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    auto filter = mLabelFilter.LabelCheckStates[Qt::Vertical];
    if (mLabelFilter.Any)
        return matchesAnyRowLabels(filter, sectionIndex);
    return matchesAllRowLabels(filter, sectionIndex);
}

bool LabelFilterModel::matchesAllColumnLabels(const LabelCheckStates &checkStates,
                                              int sectionIndex) const
{
    auto sym = mModelInstance->variable(sectionIndex);
    auto labels = sym->sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin() ;
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked) {
            continue;
        }
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool LabelFilterModel::matchesAnyColumnLabels(const LabelCheckStates &checkStates,
                                              int sectionIndex) const
{
    auto sym = mModelInstance->variable(sectionIndex);
    if (sym->isScalar()) return true;
    auto labels = sym->sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked && labels.contains(iter->first, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool LabelFilterModel::matchesAllRowLabels(const LabelCheckStates &checkStates,
                                           int sectionIndex) const
{
    auto sym = mModelInstance->equation(sectionIndex);
    auto labels = sym->sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked) {
            continue;
        }
        if (labels.contains(iter->first, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool LabelFilterModel::matchesAnyRowLabels(const LabelCheckStates &checkStates,
                                           int sectionIndex) const
{
    auto sym = mModelInstance->equation(sectionIndex);
    if (sym->isScalar()) return true;
    auto labels = sym->sectionLabels()[sectionIndex];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        if (iter->second == Qt::Checked && labels.contains(iter->first, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

}
}
}
