#ifndef COMMON_H
#define COMMON_H

#include <limits>

#include <QMap>
#include <QString>
#include <QVector>
#include <Qt>

namespace gams {
namespace studio {
namespace modelinspector {

const QStringList PredefinedHeader { "level", "lower", "marginal", "scale", "upper" };
const int PredefinedHeaderLength = PredefinedHeader.size();

struct SearchResult
{
    int Index = -1;
    Qt::Orientation Orientation;
};

struct SymbolFilterItem
{
    bool Enabled;
    int SectionIndex;
    QString Text;
    Qt::CheckState Checked;
};

typedef QVector<SymbolFilterItem> SymbolFilter;
typedef QMap<Qt::Orientation, SymbolFilter> SymbolFilterMap;

typedef QMap<QString, Qt::CheckState> UelFilter;
typedef QMap<Qt::Orientation, UelFilter> UelFilterMap;

struct ValueFilter
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

#endif // COMMON_H
