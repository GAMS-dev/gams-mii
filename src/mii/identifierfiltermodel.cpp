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
#include "identifierfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio {
namespace mii {

IdentifierFilterModel::IdentifierFilterModel(const QSharedPointer<AbstractModelInstance> &modelInstance,
                                             QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void IdentifierFilterModel::setIdentifierFilter(const IdentifierFilter &filter)
{
    mIdentifierFilter = filter;
    invalidateFilter();
}

bool IdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
                                                const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    for (auto iter=mIdentifierFilter[Qt::Horizontal].constBegin();
            iter!=mIdentifierFilter[Qt::Horizontal].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        if (iter->Text == sym->name() && sectionIndex >= sym->firstSection() &&
                sectionIndex <= sym->lastSection()) {
            return false;
        }
    }
    return true;
}

bool IdentifierFilterModel::filterAcceptsRow(int sourceRow,
                                             const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    for (auto iter=mIdentifierFilter[Qt::Vertical].constBegin();
            iter!=mIdentifierFilter[Qt::Vertical].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto sym = mModelInstance->equation(sectionIndex);
        if (iter->Text == sym->name() && sectionIndex >= sym->firstSection() &&
                sectionIndex <= sym->lastSection()) {
            return false;
        }
    }
    return true;
}

IdentifierLabelFilterModel::IdentifierLabelFilterModel(const QSharedPointer<AbstractModelInstance> &modelInstance,
                                                       QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void IdentifierLabelFilterModel::clearIdentifierFilter()
{
    mIdentifierFilter.clear();
}

IdentifierState IdentifierLabelFilterModel::identifierState(int symbolIndex,
                                                            Qt::Orientation orientation) const
{
    return mIdentifierFilter[orientation].value(symbolIndex);
}

void IdentifierLabelFilterModel::setIdentifierState(const IdentifierState &state,
                                                    Qt::Orientation orientation)
{
    mIdentifierFilter[orientation][state.SymbolIndex] = state;
    invalidateFilter();
}

bool IdentifierLabelFilterModel::filterAcceptsColumn(int sourceColumn,
                                                     const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    if (mIdentifierFilter[Qt::Horizontal].isEmpty())
        return true;
    int firstSection = mModelInstance->variable(sectionIndex)->firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Horizontal].value(firstSection);
    for (auto iter=item.CheckStates.keyValueBegin(); iter!=item.CheckStates.keyValueEnd(); ++iter) {
        if (sectionIndex == iter->first && iter->second == Qt::Unchecked) {
            return false;
        }
    }
    return true;
}

bool IdentifierLabelFilterModel::filterAcceptsRow(int sourceRow,
                                                  const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    if (mIdentifierFilter[Qt::Vertical].isEmpty())
        return true;
    int firstSection = mModelInstance->equation(sectionIndex)->firstSection();
    if (firstSection < 0) return true;
    auto item = mIdentifierFilter[Qt::Vertical].value(firstSection);
    for (auto iter=item.CheckStates.keyValueBegin(); iter!=item.CheckStates.keyValueEnd(); ++iter) {
        if (sectionIndex == iter->first && iter->second == Qt::Unchecked) {
            return false;
        }
    }
    return true;
}

}
}
}
