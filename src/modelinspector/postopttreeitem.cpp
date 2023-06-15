#include "postopttreeitem.h"
#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

const QString PostoptTreeItem::AttributeHeader = "Attributes";
const QString PostoptTreeItem::EquationHeader = "Equations";
const QString PostoptTreeItem::VariableHeader = "Variables";
const QVector<QVariant> PostoptTreeItem::AttributeRowHeader = {
                                                                Mi::Level,
                                                                Mi::Marginal,
                                                                Mi::Lower,
                                                                Mi::Upper,
                                                                Mi::Scale,
                                                                Mi::Range,
                                                                Mi::SlackLB,
                                                                Mi::SlackUB,
                                                                Mi::Slack,
                                                                Mi::Infeasibility,
                                                                Mi::Type
                                                              };
const QVector<QVariant> PostoptTreeItem::EquationLineHeader = {"Equations", "Aij", "Ui", "Aij*Ui"};
const QVector<QVariant> PostoptTreeItem::VariableLineHeader = {"Variables", "Aij", "Xj", "Aij*Xj"};

}
}
}
