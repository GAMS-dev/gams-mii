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
#ifndef VIEWCONFIGURATIONPROVIDER_H
#define VIEWCONFIGURATIONPROVIDER_H

#include "aggregation.h"
#include "common.h"

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace mii {

class AbstractViewFrame;
class AbstractModelInstance;

class AbstractViewConfiguration
{
public:
    enum Option {
        AggregationConfig   = 0x01,
        IdentifierConfig    = 0x02,
        LabelConfig         = 0x04,
        ValueConfig         = 0x08
    };
    Q_DECLARE_FLAGS(Options, Option)

    AbstractViewConfiguration(ViewHelper::ViewDataType viewType,
                              QSharedPointer<AbstractModelInstance> modelInstance = nullptr);

    virtual ~AbstractViewConfiguration()
    {

    }

    virtual AbstractViewConfiguration* clone() = 0;

    QSharedPointer<AbstractModelInstance> modelInstance() const
    {
        return mModelInstance;
    }

    void setModelInstance(QSharedPointer<AbstractModelInstance> modelInstance);

    inline int viewId() const
    {
        return mViewId;
    }

    inline void setViewId(int viewId)
    {
        mViewId = viewId;
    }

    inline ViewHelper::ViewDataType viewType() const
    {
        return mViewType;
    }

    AbstractViewFrame* view() const;

    void setView(AbstractViewFrame* view);

    Aggregation& currentAggregation()
    {
        return mCurrentAggregation;
    }

    void setCurrentAggregation(const Aggregation& aggregation)
    {
        mCurrentAggregation = aggregation;
    }

    Aggregation& defaultAggregation()
    {
        return mDefaultAggregation;
    }

    void setDefaultAggregation(const Aggregation& aggregation)
    {
        mDefaultAggregation = aggregation;
    }

    void resetAggregation()
    {
        mCurrentAggregation = mDefaultAggregation;
    }

    LabelFilter& currentLabelFiler()
    {
        return mCurrentLabelFilter;
    }

    void setCurrentLabelFilter(const LabelFilter& filter)
    {
        mCurrentLabelFilter = filter;
    }

    LabelFilter& defaultLabelFilter()
    {
        return mDefaultLabelFilter;
    }

    void setDefaultLabelFilter(const LabelFilter& filter)
    {
        mDefaultLabelFilter = filter;
    }

    void resetLabelFilter()
    {
        mCurrentLabelFilter = mDefaultLabelFilter;
    }

    IdentifierFilter& currentIdentifierFilter()
    {
        return mCurrentIdentifierFilter;
    }

    void setCurrentIdentifierFilter(const IdentifierFilter& filter)
    {
        mCurrentIdentifierFilter = filter;
    }

    const IdentifierFilter& defaultIdentifierFilter() const
    {
        return mDefaultIdentifierFilter;
    }

    void setDefaultIdentifierFilter(const IdentifierFilter& filter)
    {
        mDefaultIdentifierFilter = filter;
    }

    void resetIdentifierFilter()
    {
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    }

    void updateIdentifierFilter(const QList<Symbol *> &eqnFilter,
                                const QList<Symbol *> &varFilter);

    ValueFilter& currentValueFilter()
    {
        return mCurrentValueFilter;
    }

    void setCurrentValueFilter(const ValueFilter& filter)
    {
        mCurrentValueFilter = filter;
    }

    ValueFilter& defaultValueFilter()
    {
        return mDefaultValueFilter;
    }

    void setDefaultValueFilter(const ValueFilter& filter)
    {
        mDefaultValueFilter = filter;
    }

    void resetValueFilter()
    {
        mCurrentValueFilter = mDefaultValueFilter;
    }

    LabelCheckStates& currentAttributeFilter()
    {
        return mCurrentAttributeFilter;
    }

    void setCurrentAttributeFilter(const LabelCheckStates &filter)
    {
        mCurrentAttributeFilter = filter;
    }

    LabelCheckStates& defaultAttributeFilter()
    {
        return mDefaultAttributeFilter;
    }

    void setDefaultAttributeFilter(const LabelCheckStates &filter)
    {
        mDefaultAttributeFilter = filter;
    }

    void resetAttributeFilter()
    {
        mCurrentAttributeFilter = mDefaultAttributeFilter;
    }

    const SectionLabels& sectionLabels(Qt::Orientation orientation) const;

    SearchResult& searchResult()
    {
        return mSearchResult;
    }

protected:
    virtual IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const = 0;

    virtual void createLabelFilter();

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    SearchResult mSearchResult;

    SectionLabels mHorizontalSectionLabels;
    SectionLabels mVerticalSectionLabels;

    LabelFilter mCurrentLabelFilter;
    LabelFilter mDefaultLabelFilter;

    IdentifierFilter mCurrentIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;

    ValueFilter mCurrentValueFilter;
    ValueFilter mDefaultValueFilter;

    Aggregation mCurrentAggregation;
    Aggregation mDefaultAggregation;

    LabelCheckStates mCurrentAttributeFilter;
    LabelCheckStates mDefaultAttributeFilter;

private:
    int mViewId;
    ViewHelper::ViewDataType mViewType;
    AbstractViewFrame* mView = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractViewConfiguration::Options)

class ViewConfigurationProvider final
{
private:
    ViewConfigurationProvider() {};

public:
    static int currentViewId()
    {
        return ViewId;
    }

    static int nextViewId()
    {
        return ++ViewId;
    }

    static AbstractViewConfiguration* defaultConfiguration();

    static AbstractViewConfiguration* configuration(ViewHelper::ViewDataType viewType,
                                                    QSharedPointer<AbstractModelInstance> modelInstance);

private:
    static int ViewId;
};

}
}
}

#endif // VIEWCONFIGURATIONPROVIDER_H
