#include "postopttreeview.h"
#include "common.h"

#include <QEvent>
#include <QWheelEvent>

namespace gams {
namespace studio{
namespace modelinspector {

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
                zoomIn(Mi::ZoomFactor);
            else
                zoomOut(Mi::ZoomFactor);
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
