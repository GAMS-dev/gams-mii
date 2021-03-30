#include "sectiontreeview.h"
#include "sectiontreemodel.h"

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

SectionTreeView::SectionTreeView(QWidget *parent)
    : QTreeView(parent)
{

}

void SectionTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid())
        return;
    auto *item = static_cast<ViewItem*>(current.internalPointer());
    emit currentItemChanged(item->page());
    QTreeView::currentChanged(current, previous);
}

}
}
}
