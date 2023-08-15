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
    Unknown             = 127
};

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

    static bool isSpecialValue(const QVariant &value)
    {
        auto str = value.toString();
        return !str.compare(Mi::SV_EPS, Qt::CaseInsensitive) ||
               !str.compare(Mi::SV_INF, Qt::CaseInsensitive) ||
               !str.compare(Mi::SV_PINF, Qt::CaseInsensitive) ||
               !str.compare(Mi::SV_NINF, Qt::CaseInsensitive) ||
               !str.compare(Mi::SV_NA, Qt::CaseInsensitive);
    }

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

    static const QString SV_NA;
    static const QString SV_EPS;
    static const QString SV_INF;
    static const QString SV_PINF;
    static const QString SV_NINF;

    static const QString Infeasibility;
    static const QString Level;
    static const QString Lower;
    static const QString Marginal;
    static const QString MarginalNum;
    static const QString Range;
    static const QString Scale;
    static const QString Slack;
    static const QString SlackLB;
    static const QString SlackUB;
    static const QString Upper;
    static const QString Type;

    static const QString Jaccobian;
    static const QString BPScaling;
    static const QString BPOverview;
    static const QString BPCount;
    static const QString BPAverage;
    static const QString Postopt;
    static const QStringList PredefinedViewTexts;
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
    bool Reset = false;

    bool isAbsolute() const
    {
        return UseAbsoluteValues || UseAbsoluteValuesGlobal;
    }

    bool accepts(const QVariant &value) const
    {
        if (!value.isValid())
            return false;
        auto str = value.toString();
        if (!str.compare(Mi::SV_PINF, Qt::CaseInsensitive)) {
            return ShowPInf ? true : false;
        }
        if (!str.compare(Mi::SV_NINF, Qt::CaseInsensitive)) {
            return ShowNInf ? true : false;
        }
        if (!str.compare(Mi::SV_EPS, Qt::CaseInsensitive)) {
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
