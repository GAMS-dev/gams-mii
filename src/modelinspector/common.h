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

struct Constant
{
    const int ZoomFactor = 2;

    const QString NA = "NA";
    const QString EPS = "EPS";
    const QString INF = "INF";
    const QString P_INF = "+INF";
    const QString N_INF = "-INF";

    const QString Level = "level";
    const QString Lower = "lower";
    const QString Marginal = "marginal";
    const QString Scale = "scale";
    const QString Upper = "upper";

    const QStringList PredefinedHeader { Level, Lower, Marginal, Scale, Upper };
    const int PredefinedHeaderLength = PredefinedHeader.size();

    const QString Statistic = "Statistic";
    const QString EquationAttributes = "Equation Attributes";
    const QString VariableAttributes = "Variable Attributes";
    const QString Jaccobian = "Jaccobian";
    const QString FullView = "Full View";
    const QString MinMax = "Min Max";
    const QString SymbolView = "Symbol View";
    const QString SymbolEqnView = "Symbol Equation View";
    const QString SymbolVarView = "Symbol Variable View";
    const QStringList PredefinedViewTexts = { Statistic,
                                              EquationAttributes,
                                              VariableAttributes,
                                              Jaccobian,
                                              FullView,
                                              MinMax };
};

Q_GLOBAL_STATIC(Constant, constant);

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

    bool isUserInput() const
    {
        return mIsUserInput;
    }

    void setIsUserInput(bool userInput)
    {
        mIsUserInput = userInput;
    }

    bool accepts(const QVariant &value) const
    {
        if (!value.isValid())
            return false;
        auto str = value.toString();
        if (!str.compare(constant->P_INF, Qt::CaseInsensitive)) {
            return ShowPInf ? true : false;
        }
        if (!str.compare(constant->N_INF, Qt::CaseInsensitive)) {
            return ShowNInf ? true : false;
        }
        if (!str.compare(constant->EPS, Qt::CaseInsensitive)) {
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
        return !str.compare(constant->EPS, Qt::CaseInsensitive) ||
                !str.compare(constant->INF, Qt::CaseInsensitive) ||
                !str.compare(constant->P_INF, Qt::CaseInsensitive) ||
                !str.compare(constant->N_INF, Qt::CaseInsensitive) ||
                !str.compare(constant->NA, Qt::CaseInsensitive);
    }

private:
    bool mIsUserInput = false;
};

}
}
}

#endif // COMMON_H
