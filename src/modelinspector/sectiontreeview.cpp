#include "sectiontreeview.h"
#include "sectiontreeitem.h"

#include <QDebug>
#include <QMenu>

namespace gams {
namespace studio {
namespace modelinspector {

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

void SectionTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid())
        return;
    auto *item = static_cast<SectionTreeItem*>(current.internalPointer());
    emit currentItemChanged(item->page());
    QTreeView::currentChanged(current, previous);
}

ViewActionStates SectionTreeView::viewActionStates(const QModelIndex &index) const
{
    ViewActionStates states;
    if (index.isValid()) {
        auto *item = static_cast<SectionTreeItem*>(index.internalPointer());
        if (index.parent() == model()->index((int)ViewType::Custom, 0)) {
            states.SaveEnabled = true;
            states.RemoveEnabled = true;
        } else if (item->type() == ViewDataType::Statistic || item->type() == ViewDataType::Unknown) {
            states.SaveEnabled = false;
            states.RemoveEnabled = false;
        } else if (item->type() < ViewDataType::Unknown) {
            states.SaveEnabled = true;
            states.RemoveEnabled =false;
        }
    }
    return states;
}

}
}
}
