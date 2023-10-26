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
#include "sectiontreeview.h"
#include "sectiontreeitem.h"

#include <QDebug>
#include <QMenu>

namespace gams {
namespace studio {
namespace mii {

SectionTreeView::SectionTreeView(QWidget *parent)
    : QTreeView(parent)
    , mMenu(new QMenu(this))
    , mSaveViewAction(new QAction("Save", this))
    , mRemoveViewAction(new QAction("Remove", this))
{
    mMenu->addAction(mSaveViewAction);
    mMenu->addAction(mRemoveViewAction);

    connect(this, &SectionTreeView::customContextMenuRequested,
            this, &SectionTreeView::showCustomContextMenu);
    connect(mSaveViewAction, &QAction::triggered,
            this, &SectionTreeView::saveViewTriggered);
    connect(mRemoveViewAction, &QAction::triggered,
            this, &SectionTreeView::removeViewTriggered);
}

ViewActionStates SectionTreeView::viewActionStates() const
{
    auto index = currentIndex();
    return viewActionStates(index);
}

void SectionTreeView::showCustomContextMenu(const QPoint &pos)
{
    auto index = indexAt(pos);
    if (!index.isValid()) return;
    auto states = viewActionStates(index);
    mSaveViewAction->setEnabled(states.SaveEnabled);
    mRemoveViewAction->setEnabled(states.RemoveEnabled);
    mMenu->popup(viewport()->mapToGlobal(pos));
}

void SectionTreeView::currentChanged(const QModelIndex &current,
                                     const QModelIndex &previous)
{
    if (!current.isValid())
        return;
    emit currentItemChanged();
    QTreeView::currentChanged(current, previous);
}

ViewActionStates SectionTreeView::viewActionStates(const QModelIndex &index) const
{
    ViewActionStates states;
    if (index.isValid()) {
        auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
        if (item->isCustom()) {
            if (item->isGroup()) {
                states.SaveEnabled = false;
                states.RemoveEnabled = true;
            } else {
                states.SaveEnabled = true;
                states.RemoveEnabled = true;
            }
        } else {
            if (item->isGroup()) {
                states.SaveEnabled = false;
                states.RemoveEnabled = false;
            } else {
                states.SaveEnabled = true;
                states.RemoveEnabled = false;
            }
        }
    }
    return states;
}

}
}
}
