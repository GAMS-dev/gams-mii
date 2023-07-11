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
#include "postopttreeitem.h"
#include "common.h"

namespace gams {
namespace studio {
namespace mii {

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