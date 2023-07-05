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
namespace modelinspector {

class AbstractModelInstance;

class AbstractViewConfiguration
{
public:
    AbstractViewConfiguration(ViewDataType viewType,
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

    inline int view() const
    {
        return mView;
    }

    inline void setView(int view)
    {
        mView = view;
    }

    inline ViewDataType viewType() const
    {
        return mViewType;
    }

    Aggregation& currentAggregation()
    {
        return mCurrentAggregation;
    }

    void setCurrentAggregation(const Aggregation& aggregation)
    {
        mCurrentAggregation = aggregation;
    }

    const Aggregation& defaultAggregation() const
    {
        return mDefaultAggregation;
    }

    void setDefaultAggregation(const Aggregation& aggregation)
    {
        mDefaultAggregation = aggregation;
    }

    const LabelFilter& currentLabelFiler() const
    {
        return mCurrentLabelFilter;
    }

    void setCurrentLabelFilter(const LabelFilter& filter)
    {
        mCurrentLabelFilter = filter;
    }

    const LabelFilter& defaultLabelFilter() const
    {
        return mDefaultLabelFilter;
    }

    void setDefaultLabelFilter(const LabelFilter& filter)
    {
        mDefaultLabelFilter = filter;
    }

    IdentifierFilter& currentIdentifierFilter()
    {
        return mCurrentIdentifierFilter;
    }

    void setCurrentIdentifierFilter(const IdentifierFilter& filter)
    {
        mCurrentIdentifierFilter = filter;
    }

    const IdentifierFilter& defaultIdentifierFilter() const {
        return mDefaultIdentifierFilter;
    }

    void setDefaultIdentifierFilter(const IdentifierFilter& filter)
    {
        mDefaultIdentifierFilter = filter;
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

    ValueFilter& defaultValueFilter() {
        return mDefaultValueFilter;
    }

    void setDefaultValueFilter(const ValueFilter& filter)
    {
        mDefaultValueFilter = filter;
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
    int mView;

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

private:
    ViewDataType mViewType;
};

class ViewConfigurationProvider final
{
private:
    ViewConfigurationProvider() {};

public:
    static AbstractViewConfiguration* defaultConfiguration();

    static AbstractViewConfiguration* configuration(ViewDataType viewType,
                                                    QSharedPointer<AbstractModelInstance> modelInstance);
};

}
}
}

#endif // VIEWCONFIGURATIONPROVIDER_H
