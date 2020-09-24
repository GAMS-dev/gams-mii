#include "statisticedit.h"
#include "modelstatistic.h"

#include <QToolTip>
#include <QEvent>
#include <QHelpEvent>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

StatisticEdit::StatisticEdit(QWidget *parent)
    : QTextEdit(parent)
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

        // TODO =E=,... not working... because it is not a word
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

void StatisticEdit::showStatistic(const ModelStatistic &statistics)
{
    auto matrixRange = statistics.matrixRange();
    auto objectiveRange = statistics.objectiveRange();
    auto boundsRange = statistics.boundsRange();
    auto rhsRange = statistics.rhsRange();

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
                        "<td align=\"right\">" + QString::number(statistics.equationBlocks()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.equations()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_E)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_G)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_L)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_N)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_X)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_C)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberEquations(gmoequ_B)) + "</td>"
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
                        "<td align=\"right\">" + QString::number(statistics.variableBlocks()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.variables()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_X)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_B)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_I)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_S1)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_S2)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_SC)) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.numberVariables(gmovar_SI)) + "</td>"
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
                        "<td align=\"right\">" + QString::number(statistics.coefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.positiveCoefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.negativeCoefficents()) + "</td>"
                        "<td align=\"right\">" + QString::number(statistics.nonLinearCoefficents()) + "</td>"
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

}
}
}
