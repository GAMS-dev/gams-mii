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
#ifndef COMMON_H
#define COMMON_H

#include <limits>

#include <QMap>
#include <QString>
#include <QVariant>
#include <QVector>

namespace gams {
namespace studio {
namespace mii {

class AttributeHelper
{
public:
    enum AttributeType
    {
        Level,
        /// Represents numerical and special values (as text).
        Marginal,
        /// Represents the numerical value.
        MarginalNum,
        Lower,
        Upper,
        Scale,
        Range,
        SlackLB,
        SlackUB,
        Slack,
        Infeasibility,
        Type
    };

    static QString attributeText(AttributeType type)
    {
        switch (type) {
        case Level:
            return LevelText;
        case Marginal:
            return MarginalText;
        case MarginalNum:
            return MarginalNumText;
        case Lower:
            return LowerText;
        case Upper:
            return UpperText;
        case Scale:
            return ScaleText;
        case Range:
            return RangeText;
        case SlackLB:
            return SlackLBText;
        case SlackUB:
            return SlackUBText;
        case Slack:
            return SlackText;
        case Infeasibility:
            return InfeasibilityText;
        default:
            return TypeText;
        }
    }

    static QVector<QString> attributeTextList()
    {
        return QVector<QString> {
            LevelText,
            MarginalText,
            LowerText,
            UpperText,
            ScaleText,
            RangeText,
            SlackLBText,
            SlackUBText,
            SlackText,
            InfeasibilityText,
            TypeText
        };
    }

    static double attributeValue(double a, double b, bool aInf = false, bool bInf = false)
    {
        if (aInf || bInf) {
            if (aInf && !bInf) {
                return a;
            } else if (!aInf && bInf) {
                return b;
            } else if ((a < 0 && b < 0) || (a > 0 && b > 0)) {
                return a;
            } else {
                return a + b;
            }
        }
        return a - b;
    }

    static const QString InfeasibilityText;
    static const QString LevelText;
    static const QString LowerText;
    static const QString MarginalText;
    static const QString MarginalNumText;
    static const QString RangeText;
    static const QString ScaleText;
    static const QString SlackText;
    static const QString SlackLBText;
    static const QString SlackUBText;
    static const QString UpperText;
    static const QString TypeText;
};

class ValueHelper
{
public:
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

    enum class SpecialValueType
    {
        NA,
        EPS,
        INF,
        P_INF,
        N_INF
    };

    static QString specialValueText(SpecialValueType type)
    {
        switch (type) {
        case SpecialValueType::EPS:
            return EPSText;
        case SpecialValueType::INF:
            return INFText;
        case SpecialValueType::P_INF:
            return PINFText;
        case SpecialValueType::N_INF:
            return NINFText;
        default:
            return NAText;
        }
    }

    static bool isSpecialValue(const QVariant &value)
    {
        auto str = value.toString();
        return !str.compare(EPSText, Qt::CaseInsensitive) ||
               !str.compare(INFText, Qt::CaseInsensitive) ||
               !str.compare(PINFText, Qt::CaseInsensitive) ||
               !str.compare(NINFText, Qt::CaseInsensitive) ||
               !str.compare(NAText, Qt::CaseInsensitive);
    }

    static const QString NAText;
    static const QString EPSText;
    static const QString INFText;
    static const QString PINFText;
    static const QString NINFText;

    static const unsigned char Plus;
    static const unsigned char Minus;
    static const unsigned char Mixed;
    static const QString PlusMinus;
};

class ViewHelper
{
public:
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

    enum class ViewType
    {
        Predefined  = 0,
        Custom      = 1
    };

    enum class ViewDataType
    {
        BP_Overview         = 0,
        BP_Count            = 1,
        BP_Average          = 2,
        BP_Scaling          = 3,
        Postopt             = 4,
        Symbols             = 5,
        Blockpic            = 126,
        Unknown             = 127
    };

    static bool isAggregatable(ViewDataType type)
    {
        switch (type) {
        case ViewDataType::BP_Overview:
        case ViewDataType::BP_Count:
        case ViewDataType::BP_Average:
        case ViewDataType::BP_Scaling:
        case ViewDataType::Postopt:
        case ViewDataType::Unknown:
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

    static const int ZoomFactor = 2;

    static const QString AttributeHeaderText;
    static const QString EquationHeaderText;
    static const QString VariableHeaderText;

    static const QString ModelInstance;
    static const QString PredefinedViews;
    static const QString CustomViews;
    static const QString Blockpic;
    static const QString SymbolView;

    static const QString Jacobian;
    static const QString BPScaling;
    static const QString BPOverview;
    static const QString BPCount;
    static const QString BPAverage;
    static const QString Postopt;
    static const QStringList PredefinedViewTexts;
};

class FileHelper
{
public:
    static const QString GamsCntr;
    static const QString GamsDict;
    static const QString Gamsmatr;
    static const QString GamsSolu;
    static const QString GamsStat;
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

    bool operator==(const IdentifierState& other) const
    {
        return Enabled == other.Enabled &&
               SectionIndex == other.SectionIndex &&
               SymbolIndex == other.SymbolIndex &&
               Text == other.Text &&
               Checked == other.Checked &&
               CheckStates == other.CheckStates;
    }

    bool operator!=(const IdentifierState& other) const
    {
        return !(*this == other);
    }
};

typedef QMap<int, IdentifierState> IdentifierStates;
typedef QMap<Qt::Orientation, IdentifierStates> IdentifierFilter;

typedef QMap<Qt::Orientation, LabelCheckStates> LabelStates;

struct LabelFilter
{
    bool Any = false;
    LabelStates LabelCheckStates;

    bool operator==(const LabelFilter& other) const
    {
        return Any == other.Any &&
               LabelCheckStates == other.LabelCheckStates;
    }

    bool operator!=(const LabelFilter& other) const
    {
        return !(*this == other);
    }
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

    bool isAbsolute() const
    {
        return UseAbsoluteValues || UseAbsoluteValuesGlobal;
    }

    bool accepts(const QVariant &value) const
    {
        if (!value.isValid())
            return false;
        auto str = value.toString();
        if (!str.compare(ValueHelper::PINFText, Qt::CaseInsensitive)) {
            return ShowPInf ? true : false;
        }
        if (!str.compare(ValueHelper::NINFText, Qt::CaseInsensitive)) {
            return ShowNInf ? true : false;
        }
        if (!str.compare(ValueHelper::EPSText, Qt::CaseInsensitive)) {
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

    bool operator==(const ValueFilter& other) const
    {
        return MinValue == other.MinValue && MaxValue == other.MaxValue &&
               ExcludeRange == other.ExcludeRange &&
               UseAbsoluteValues == other.UseAbsoluteValues &&
               UseAbsoluteValuesGlobal == other.UseAbsoluteValuesGlobal &&
               ShowPInf == other.ShowPInf && ShowNInf == other.ShowNInf &&
               ShowEps == other.ShowEps;
    }

    bool operator!=(const ValueFilter& other) const
    {
        return !(*this == other);
    }
};

}
}
}

#endif // COMMON_H
