
#include "statisticedit.h"
#include "abstractmodelinstance.h"

#include <QToolTip>
#include <QEvent>
#include <QHelpEvent>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

StatisticEdit::StatisticEdit(QWidget *parent)
    : QTextEdit(parent)
    , mBaseFont(font())
{
    setMouseTracking(true);
}

bool StatisticEdit::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        event->accept();
        auto helpEvent = static_cast<QHelpEvent*>(event);

        QPoint pos = helpEvent->pos();
        pos.setX(pos.x() - viewportMargins().left());
        pos.setY(pos.y() - viewportMargins().right());

        QTextCursor cursor = cursorForPosition(helpEvent->pos());
        cursor.select(QTextCursor::WordUnderCursor);
        auto text = cursor.selectedText().toLower();
        if (text.isEmpty()) {
            QToolTip::hideText();
        } else if (mToolTipMapping.contains(text)) {
            QToolTip::showText(helpEvent->globalPos(), mToolTipMapping[text]);
        }
        return true;
    }

    return QTextEdit::event(event);
}

bool StatisticEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::Wheel)
        return QTextEdit::eventFilter(watched, event);
    QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
    if (wheel->modifiers() == Qt::ControlModifier) {
        if (wheel->angleDelta().y() > 0)
            zoomIn(2);
        else
            zoomOut(2);
    }
    return true;
}

void StatisticEdit::resetZoom()
{
    setFont(mBaseFont);
}

void StatisticEdit::showStatistic(const QSharedPointer<AbstractModelInstance> &modelInstance)
{
    auto matrixRange = modelInstance->matrixRange();
    auto objectiveRange = modelInstance->objectiveRange();
    auto boundsRange = modelInstance->boundsRange();
    auto rhsRange = modelInstance->rhsRange();

    QString html("<h2>Equation Counts</h2>"
                 "<div><table>"
                    "<tr>"
                        "<th>Blocks</th>"
                        "<th>Individuals</th>"
                        "<th>=E=</th>"
                        "<th>=G=</th>"
                        "<th>=L=</th>"
                        "<th>=N=</th>"
                        "<th>=X=</th>"
                        "<th>=C=</th>"
                        "<th>=B=</th>"
                    "</tr>"
                    "<tr>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationRowCount()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::E)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::G)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::L)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::N)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::X)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::C)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equationCount(EquationType::B)) + "</td>"
                    "</tr>"
                 "</table></div>"
                 "<h2>Variable Counts</h2>"
                 "<div><table>"
                    "<tr>"
                        "<th>Blocks</th>"
                        "<th>Individuals</th>"
                        "<th>Continous</th>"
                        "<th>Binary</th>"
                        "<th>Integer</th>"
                        "<th>SOS1</th>"
                        "<th>SOS2</th>"
                        "<th>SCont</th>"
                        "<th>SInt</th>"
                    "</tr>"
                    "<tr>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableRowCount()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::X)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::B)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::I)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::S1)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::S2)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::SC)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variableCount(VariableType::SI)) + "</td>"
                    "</tr>"
                 "</table></div>"
                 "<h2>Coefficient Counts</h2>"
                 "<div><table>"
                    "<tr>"
                        "<th>Total</th>"
                        "<th>Positive</th>"
                        "<th>Negative</th>"
                        "<th>Non-Linear</th>"
                    "</tr>"
                    "<tr>"
                        "<td align=\"right\">" + QString::number(modelInstance->coefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->positiveCoefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->negativeCoefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->nonLinearCoefficents()) + "</td>"
                    "</tr>"
                 "</table></div>"
                 "<h2>Coefficent Statistics</h2></div>"
                 "<div><table>"
                    "<tr>"
                        "<th>Matrix Range</th>"
                        "<th>Objective Range</th>"
                        "<th>Bounds Range</th>"
                        "<th>RHS Range</th>"
                    "</tr>"
                    "<tr>"
                        "<td align=\"right\">" + QString::number(matrixRange.first) + " - " +    QString::number(matrixRange.second) + "</td>"
                        "<td align=\"right\">" + QString::number(objectiveRange.first) + " - " + QString::number(objectiveRange.second) + "</td>"
                        "<td align=\"right\">" + QString::number(boundsRange.first) + " - " +    QString::number(boundsRange.second) + "</td>"
                        "<td align=\"right\">" + QString::number(rhsRange.first) + " - " +       QString::number(rhsRange.second) + "</td>"
                    "</tr>"
                 "</table>");
    setHtml(html);
}

void StatisticEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier ||
            event->modifiers() == (Qt::ControlModifier | Qt::KeypadModifier)) {
        if (event->key() == Qt::Key_Plus) {
            zoomIn(2);
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Minus) {
            zoomOut(2);
            event->accept();
            return;
        } else if (event->key() == Qt::Key_0) {
            setFont(mBaseFont);
            event->accept();
            return;
        }
    }
    QTextEdit::keyPressEvent(event);
}

}
}
}
