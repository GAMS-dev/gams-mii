#include "statisticedit.h"
#include "modelinstance.h"

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

void StatisticEdit::showStatistic(const QSharedPointer<ModelInstance> &modelInstance)
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
                        "<td align=\"right\">" + QString::number(modelInstance->equationBlocks()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_E)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_G)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_L)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_N)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_X)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_C)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->equations(gmoequ_B)) + "</td>"
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
                        "<td align=\"right\">" + QString::number(modelInstance->variableBlocks()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables()) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_X)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_B)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_I)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_S1)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_S2)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_SC)) + "</td>"
                        "<td align=\"right\">" + QString::number(modelInstance->variables(gmovar_SI)) + "</td>"
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

}
}
}
