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
#include "postopttreeview.h"
#include "postopttreeitem.h"
#include "common.h"

#include <QEvent>
#include <QWheelEvent>

namespace gams {
namespace studio{
namespace mii {

PostoptTreeView::PostoptTreeView(QWidget *parent)
    : QTreeView(parent)
{

}

bool PostoptTreeView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if (wheel->modifiers() == Qt::ControlModifier) {
            if (wheel->angleDelta().y() > 0)
                zoomIn(ViewHelper::ZoomFactor);
            else
                zoomOut(ViewHelper::ZoomFactor);
            return true;
        }
    }
    return QTreeView::eventFilter(watched, event);
}

void PostoptTreeView::zoomIn(int range)
{
    zoom(range);
    resizeColumns();
}

void PostoptTreeView::zoomOut(int range)
{
    zoom(-range);
    resizeColumns();
}

void PostoptTreeView::resetZoom()
{
    setFont(mBaseFont);
    resizeColumns();
}

void PostoptTreeView::mousePressEvent(QMouseEvent *event)
{
    auto idx = indexAt(event->pos());
    if (idx.isValid()) {
        auto item = static_cast<PostoptTreeItem*>(idx.internalPointer());
        if (item->type() == PostoptTreeItem::ClickItem) {
            emit openFilterDialog();
        }
    }
    QTreeView::mousePressEvent(event);
}

void PostoptTreeView::zoom(int range)
{
    if (range == 0.f)
        return;
    QFont f = font();
    const float newSize = f.pointSizeF() + range;
    if (newSize <= 0)
        return;
    f.setPointSize(newSize);
    setFont(f);
    resizeColumns();
}

void PostoptTreeView::resizeColumns()
{
    for (int i=0; i<model()->columnCount(); ++i) {
        resizeColumnToContents(i);
    }
}

}
}
}
