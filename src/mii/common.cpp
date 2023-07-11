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
#include "common.h"

namespace gams {
namespace studio {
namespace mii {

const QString Mi::SV_NA     = "NA";
const QString Mi::SV_EPS    = "EPS";
const QString Mi::SV_INF    = "INF";
const QString Mi::SV_PINF   = "+INF";
const QString Mi::SV_NINF   = "-INF";

const QString Mi::Infeasibility = "Infeasibility";
const QString Mi::Level         = "Level";
const QString Mi::Lower         = "Lower";
const QString Mi::Marginal      = "Marginal";
const QString Mi::MarginalNum   = "MarginalNum";
const QString Mi::Range         = "Range";
const QString Mi::Scale         = "Scale";
const QString Mi::Slack         = "Slack";
const QString Mi::SlackLB       = "Slack lower bound";
const QString Mi::SlackUB       = "Slack upper bound";
const QString Mi::Upper         = "Upper";
const QString Mi::Type          = "Type";

const QString Mi::Jaccobian     = "Jaccobian";
const QString Mi::BPScaling     = "Blockpic Scaling";
const QString Mi::BPOverview    = "Blockpic Overview";
const QString Mi::BPCount       = "Blockpic Count";
const QString Mi::BPAverage     = "Blockpic Average";
const QString Mi::Postopt       = "Postopt";
const QStringList Mi::PredefinedViewTexts = {
                                                Jaccobian,
                                                BPOverview,
                                                BPCount,
                                                BPAverage,
                                                BPScaling,
                                                Postopt
                                            };

}
}
}
