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

    const QString Jaccobian = "Jaccobian";
    const QString BPScaling = "Blockpic Scaling";
    const QString BPOverview = "Blockpic Overview";
    const QString BPCount    = "Blockpic Count";
    const QString BPAverage  = "Blockpic Average";
    const QStringList PredefinedViewTexts = { Jaccobian,
                                              BPOverview,
                                              BPCount,
                                              BPAverage,
                                              BPScaling
                                              };
};

Q_GLOBAL_STATIC(Constant, constant);

struct Mi
{
    enum ItemDataRole
    {
        IndexDataRole = Qt::UserRole,
        LabelDataRole,
        RowEntryRole,
        ColumnEntryRole,
        HorizontalDimensionRole,
        VerticalDimensionRole,
        SectionLabelRole
    };

    static QHash<int, QByteArray> roleNames()
    {
        static QHash<int, QByteArray> mapping {
            {IndexDataRole, "indexdata"},
            {LabelDataRole, "labeldata"},
            {RowEntryRole, "rowentry"},
            {ColumnEntryRole, "columnentry"},
            {HorizontalDimensionRole, "horizontaldimension"},
            {VerticalDimensionRole, "verticaldimension"},
            {SectionLabelRole, "sectionlabel"}
        };
        return mapping;
    }
};

enum class ViewType
{
    Predefined  = 0,
    Custom      = 1
};

enum class ViewDataType
{
    Jaccobian           = 0,
    BP_Scaling          = 1,
    BP_Overview         = 2,
    BP_Count            = 3,
    BP_Average          = 4,
    Symbols             = 5,
    Unknown             = 127
};

class ViewProperties
{// TODO add enums ViewDataType/ViewType?
public:
    static bool isAggregatable(ViewDataType type)
    {
        switch (type) {
        case ViewDataType::BP_Overview:
        case ViewDataType::BP_Count:
        case ViewDataType::BP_Average:
        case ViewDataType::BP_Scaling:
            return false;
        default:
            return true;
        }
    }

    static bool isStandardView(ViewDataType type)
    {
        switch (type) {
        case ViewDataType::BP_Count:
        case ViewDataType::BP_Average:
        case ViewDataType::BP_Scaling:
            return false;
        default:
            return true;
        }
    }
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
typedef QHash<int, QStringList> SectionLabels;

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
typedef QHash<QString, Qt::CheckState> LabelCheckStates;

struct SearchResult
{
    struct SearchEntry
    {
        int Index = -1;
        Qt::Orientation Orientation = Qt::Horizontal;

        bool operator==(const SearchEntry& other) const
        {
            return Index == other.Index && Orientation == other.Orientation;
        }

        bool operator!=(const SearchEntry& other) const
        {
            return !(*this == other);
        }
    };

    bool operator==(const SearchResult& other) const
    {
        return Term == other.Term && IsRegEx == other.IsRegEx &&
               Entries == other.Entries;
    }

    bool operator!=(const SearchResult& other) const
    {
        return !(*this == other);
    }

    QString Term;
    bool IsRegEx = false;
    QList<SearchEntry> Entries;
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

    bool isAbsolute() const
    {
        return UseAbsoluteValues || UseAbsoluteValuesGlobal;
    }

    void setIsUserInput(bool userInput)
    {
        mIsUserInput = userInput;
    }

    //bool accepts(const QVariant &value) const
    //{// only needed for attributes?
    //    if (!value.isValid())
    //        return false;
    //    auto str = value.toString();
    //    if (!str.compare(constant->P_INF, Qt::CaseInsensitive)) {
    //        return ShowPInf ? true : false;
    //    }
    //    if (!str.compare(constant->N_INF, Qt::CaseInsensitive)) {
    //        return ShowNInf ? true : false;
    //    }
    //    if (!str.compare(constant->EPS, Qt::CaseInsensitive)) {
    //        return ShowEps ? true : false;
    //    }
    //    bool ok;
    //    double val = UseAbsoluteValues ? abs(value.toDouble(&ok))
    //                                   : value.toDouble(&ok);
    //    if (!ok)
    //        return false;
    //    if (ExcludeRange) {
    //        return val < MinValue || val > MaxValue;
    //    } else {
    //        return val >= MinValue && val <= MaxValue;
    //    }
    //    return false;
    //}

    static double minValue(double value, double newValue)
    {// TODO !!! remove or improve for attributes
        if (value == 0.0) {
            return newValue;
        } else if (newValue == 0.0) {
            return value;
        }
        return std::min(value, newValue);
    }

    static double maxValue(double value, double newValue)
    {// TODO !!! remove or improve for attributes
        if (value == 0.0) {
            return newValue;
        } else if (newValue == 0.0) {
            return value;
        }
        return std::max(value, newValue);
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

    bool operator==(const ValueFilter& other) const
    {
        return MinValue == other.MinValue && MaxValue == other.MaxValue &&
               ExcludeRange == other.ExcludeRange &&
               UseAbsoluteValues == other.UseAbsoluteValues &&
               UseAbsoluteValuesGlobal == other.UseAbsoluteValuesGlobal &&
               ShowPInf == other.ShowPInf && ShowNInf == other.ShowNInf &&
               ShowEps == other.ShowEps && mIsUserInput == other.mIsUserInput;
    }

    bool operator!=(const ValueFilter& other) const
    {
        return !(*this == other);
    }

private:
    bool mIsUserInput = false;
};

}
}
}

#endif // COMMON_H
