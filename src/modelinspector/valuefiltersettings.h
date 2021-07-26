#ifndef VALUEFILTERSETTINGS_H
#define VALUEFILTERSETTINGS_H

#include <limits>

#include "filtertreeitem.h"

namespace gams {
namespace studio {
namespace modelinspector {

struct ValueFilterSettings
{
    double MinValue = std::numeric_limits<double>::min();
    double MaxValue = std::numeric_limits<double>::max();
    bool Exclude = false;

    bool ShowPInf = true;
    bool ShowNInf = true;
    bool ShowEps = true;
};

}
}
}

#endif // VALUEFILTERSETTINGS_H
