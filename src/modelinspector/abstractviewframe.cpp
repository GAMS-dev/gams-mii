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
#include "abstractviewframe.h"
#include "viewconfigurationprovider.h"

namespace gams {
namespace studio {
namespace modelinspector {

AbstractViewFrame::AbstractViewFrame(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{

}

AbstractViewFrame::~AbstractViewFrame()
{

}

const IdentifierFilter &AbstractViewFrame::identifierFilter() const
{
    return mViewConfig->currentIdentifierFilter();
}

const IdentifierFilter &AbstractViewFrame::defaultIdentifierFilter() const
{
    return mViewConfig->defaultIdentifierFilter();
}

void AbstractViewFrame::setDefaultIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setDefaultIdentifierFilter(filter);
}

const ValueFilter &AbstractViewFrame::valueFilter() const
{
    return mViewConfig->currentValueFilter();
}

const ValueFilter &AbstractViewFrame::defaultValueFilter() const
{
    return mViewConfig->defaultValueFilter();
}

void AbstractViewFrame::setValueFilter(const ValueFilter &filter)
{
    mViewConfig->setCurrentValueFilter(filter);
}

void AbstractViewFrame::setDefaultValueFilter(const ValueFilter &filter)
{
    return mViewConfig->setDefaultValueFilter(filter);
}

const LabelFilter &AbstractViewFrame::labelFilter() const
{
    return mViewConfig->currentLabelFiler();
}

const LabelFilter &AbstractViewFrame::defaultLabelFilter() const
{
    return mViewConfig->defaultLabelFilter();
}

void AbstractViewFrame::setLabelFilter(const LabelFilter &filter)
{
    mViewConfig->setCurrentLabelFilter(filter);
}

void AbstractViewFrame::setDefaultLabelFilter(const LabelFilter &filter)
{
    mViewConfig->setDefaultLabelFilter(filter);
}

const Aggregation &AbstractViewFrame::currentAggregation() const
{
    return mViewConfig->currentAggregation();
}

const Aggregation &AbstractViewFrame::defaultAggregation() const
{
    return mViewConfig->defaultAggregation();
}

void AbstractViewFrame::setDefaultAggregation(const Aggregation &aggregation)
{
    mViewConfig->setDefaultAggregation(aggregation);
}

SearchResult &AbstractViewFrame::searchResult()
{
    return mViewConfig->searchResult();
}

int AbstractViewFrame::view() const
{
    return mViewConfig->view();
}

void AbstractViewFrame::setView(int view)
{
    mViewConfig->setView(view);
}

QSharedPointer<AbstractViewConfiguration> AbstractViewFrame::viewConfig() const
{
    return mViewConfig;
}

void AbstractViewFrame::setViewConfig(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    mViewConfig = viewConfig;
}

}
}
}
