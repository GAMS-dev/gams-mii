#include "logview.h"
#include "modelinspector/common.h"

#include <QEvent>
#include <QWheelEvent>

LogView::LogView(QWidget *parent)
    : QTextEdit(parent)
    , mBaseFont(font())
{
}

bool LogView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::Wheel)
        return QTextEdit::eventFilter(watched, event);
    QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
    if (wheel->modifiers() == Qt::ControlModifier) {
        if (wheel->angleDelta().y() > 0)
            zoomIn(gams::studio::modelinspector::ZoomFactor);
        else
            zoomOut(gams::studio::modelinspector::ZoomFactor);
    }
    return true;
}

void LogView::resetZoom()
{
    setFont(mBaseFont);
}
