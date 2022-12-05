#ifndef COMMON_H
#define COMMON_H

#include <limits>

#include <QMap>
#include <QString>
#include <QVariant>
#include <QVector>

namespace gams {
namespace studio {
namespace modelinspector {

static const int ZoomFactor = 2;

static const QString NA = "NA";
static const QString EPS = "EPS";
static const QString INF = "INF";
static const QString P_INF = "+INF";
static const QString N_INF = "-INF";

static const QString Level = "level";
static const QString Lower = "lower";
static const QString Marginal = "marginal";
static const QString Scale = "scale";
static const QString Upper = "upper";

static const QStringList PredefinedHeader { Level, Lower, Marginal, Scale, Upper };
static const int PredefinedHeaderLength = PredefinedHeader.size();

static const QString Statistic = "Statistic";
static const QString EquationAttributes = "Equation Attributes";
static const QString VariableAttributes = "Variable Attributes";
static const QString Jaccobian = "Jaccobian";
static const QString FullView = "Full View";
static const QString MinMax = "Min Max";
static const QString SymbolView = "Symbol View";
static const QString SymbolEqnView = "Symbol Equation View";
static const QString SymbolVarView = "Symbol Variable View";
static const QStringList PredefinedViewTexts = { Statistic,
                                                 EquationAttributes,
                                                 VariableAttributes,
                                                 Jaccobian,
                                                 FullView,
                                                 MinMax };

enum class ViewType
{
    Predefined  = 0,
    Custom      = 1
};

enum class PredefinedViewEnum
{
    Statistic       = 0,
    EqnAttributes   = 1,
    VarAttributes   = 2,
    Jaccobian       = 3,
    Full            = 4,
    MinMax          = 5,
    SymbolView      = 6,
    SymbolEqnView   = 7,
    SymbolVarView   = 8,
    Unknown         = 127
};

enum class EquationType
{
    E,
    G,
    L,
    N,
    X,
    C,
    B
};

enum class VariableType
{
    X,
    B,
    I,
    S1,
    S2,
    SC,
    SI
};

struct ViewActionStates
{
    bool SaveEnabled = true;
    bool RemoveEnabled = true;
    bool RenameEnabled = true;
};

///
/// \brief Symbol domain labels.
///
typedef QVector<QString> DomainLabels;

///
/// \brief Labels by section index.
///
/// \remark List index is symbol dimension.
///
typedef QMap<int, QStringList> SectionLabels;

///
/// \brief A set of section which will be united during aggregation.
///
typedef QVector<QSet<int>> UnitedSections;

///
/// \brief Check state by index, like dimension or section index.
///
typedef QMap<int, Qt::CheckState> IndexCheckStates;

///
/// \brief Check states by label.
///
typedef QMap<QString, Qt::CheckState> LabelCheckStates;

struct SearchResult
{
    int Index = -1;
    Qt::Orientation Orientation = Qt::Horizontal;
};

struct IdentifierState
{
    ///
    /// \brief Enable tree selection.
    ///
    bool Enabled = false;

    int SectionIndex = -1;

    int SymbolIndex = -1;

    QString Text;
    Qt::CheckState Checked = Qt::Unchecked;
    IndexCheckStates CheckStates;

    bool isValid() const
    {
        return SectionIndex != -1 &&
                SymbolIndex != -1 &&
                !CheckStates.isEmpty();
    }

    ///
    /// \brief Returns if all labels are disabled.
    /// \return <c>true</c> if all visible labels are disabled; otherwise <c>false</c>.
    ///
    bool disabled() const
    {
        Q_FOREACH(auto state, CheckStates)
            if (state == Qt::Checked) return false;
        return !CheckStates.isEmpty();
    }

    void unite(const IdentifierState &other)
    {
        CheckStates = other.CheckStates;
        SectionIndex = other.SectionIndex;
    }
};

typedef QMap<int, IdentifierState> IdentifierStates;
typedef QMap<Qt::Orientation, IdentifierStates> IdentifierFilter;

typedef QMap<Qt::Orientation, LabelCheckStates> LabelStates;

struct LabelFilter
{
    bool Any = false;
    LabelStates LabelCheckStates;
};

struct ValueFilter
{
    double MinValue = std::numeric_limits<double>::lowest();
    double MaxValue = std::numeric_limits<double>::max();
    bool ExcludeRange = false;
    bool UseAbsoluteValues = false;
    bool UseAbsoluteValuesGlobal = false;

    bool ShowPInf = true;
    bool ShowNInf = true;
    bool ShowEps = true;

    bool accepts(const QVariant &value) const
    {
        if (!value.isValid())
            return false;
        auto str = value.toString();
        if (!str.compare(P_INF, Qt::CaseInsensitive)) {
            return ShowPInf ? true : false;
        }
        if (!str.compare(N_INF, Qt::CaseInsensitive)) {
            return ShowNInf ? true : false;
        }
        if (!str.compare(EPS, Qt::CaseInsensitive)) {
            return ShowEps ? true : false;
        }
        bool ok;
        double val = UseAbsoluteValues ? abs(value.toDouble(&ok))
                                       : value.toDouble(&ok);
        if (!ok)
            return false;
        if (ExcludeRange) {
            return val < MinValue || val > MaxValue;
        } else {
            return val >= MinValue && val <= MaxValue;
        }
        return false;
    }

    static double minValue(double value, double newValue)
    {
        if (value == 0.0) {
            return newValue;
        } else if (newValue != 0.0) {
            return std::min(value, newValue);
        }
        return value;
    }

    static double maxValue(double value, double newValue)
    {
        if (value == 0.0) {
            return newValue;
        } else if (newValue != 0.0) {
            return std::max(value, newValue);
        }
        return value;
    }

    static bool isSpecialValue(const QVariant &value)
    {
        auto str = value.toString();
        return !str.compare(EPS, Qt::CaseInsensitive) ||
                !str.compare(INF, Qt::CaseInsensitive) ||
                !str.compare(P_INF, Qt::CaseInsensitive) ||
                !str.compare(N_INF, Qt::CaseInsensitive) ||
                !str.compare(NA, Qt::CaseInsensitive);
    }
};

}
}
}

#endif // COMMON_H
