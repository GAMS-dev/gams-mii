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
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if (wheel->modifiers() == Qt::ControlModifier) {
            if (wheel->angleDelta().y() > 0)
                zoomIn(gams::studio::modelinspector::Mi::ZoomFactor);
            else
                zoomOut(gams::studio::modelinspector::Mi::ZoomFactor);
            return true;
        }
    }
    return QTextEdit::eventFilter(watched, event);
}

void LogView::resetZoom()
{
    setFont(mBaseFont);
}
