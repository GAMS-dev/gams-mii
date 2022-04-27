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
static const QStringList PredefinedViews = { Statistic, EquationAttributes,
                                             VariableAttributes, Jaccobian, FullView };

enum class PredefinedViewEnum
{
    Attributes,
    Full,
    Jaccobian,
    Statistic,
    Unknown,
    UserView
};

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

typedef QMap<int, QVariant> DataRow;
typedef QMap<int, DataRow> DataMatrix;

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
    /// \brief Retures if all labels are disabled.
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
    double MinValue = std::numeric_limits<double>::min();
    double MaxValue = std::numeric_limits<double>::max();
    bool ExcludeRange = false;
    bool UseAbsoluteValues = false;

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
};

}
}
}

#endif // COMMON_H
