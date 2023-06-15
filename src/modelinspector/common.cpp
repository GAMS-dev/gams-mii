#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

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
