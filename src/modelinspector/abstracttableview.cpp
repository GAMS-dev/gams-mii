#include "abstracttableview.h"
#include "common.h"

#include <QEvent>
#include <QWheelEvent>

namespace gams {
namespace studio{
namespace modelinspector {

AbstractTableView::AbstractTableView(QWidget *parent)
    : QTableView(parent)
{

}

bool AbstractTableView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if (wheel->modifiers() == Qt::ControlModifier) {
            if (wheel->angleDelta().y() > 0)
                zoomIn(constant->ZoomFactor);
            else
                zoomOut(constant->ZoomFactor);
            return true;
        }
    }
    return QTableView::eventFilter(watched, event);
}

void AbstractTableView::zoomIn(int range)
{
    zoom(range);
    resizeColumnsToContents();
    resizeRowsToContents();
}

void AbstractTableView::zoomOut(int range)
{
    zoom(-range);
    resizeColumnsToContents();
    resizeRowsToContents();
}

void AbstractTableView::resetZoom()
{
    setFont(mBaseFont);
    resizeColumnsToContents();
    resizeRowsToContents();
}

void AbstractTableView::zoom(int range)
{
    if (range == 0.f)
        return;
    QFont f = font();
    const float newSize = f.pointSizeF() + range;
    if (newSize <= 0)
        return;
    f.setPointSize(newSize);
    setFont(f);
    resizeColumnsToContents();
    resizeRowsToContents();
}

}
}
}
