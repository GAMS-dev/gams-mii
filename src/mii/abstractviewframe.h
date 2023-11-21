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
#ifndef ABSTRACTVIEWFRAME_H
#define ABSTRACTVIEWFRAME_H

#include "common.h"
#include "viewconfigurationprovider.h"

#include <QFrame>

namespace gams {
namespace studio {
namespace mii {

class Aggregation;
class AbstractModelInstance;

class AbstractViewFrame : public QFrame
{
    Q_OBJECT

public:
    AbstractViewFrame(QWidget *parent = nullptr,
                      Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractViewFrame();

    virtual AbstractViewFrame* clone(int viewId) = 0;

    virtual void setShowAbsoluteValues(bool absoluteValues) = 0;

    virtual ViewHelper::ViewDataType type() const = 0;

    void updateFilters(AbstractViewConfiguration::Options options);

    virtual void updateView() = 0;

    virtual void zoomIn() = 0;

    virtual void zoomOut() = 0;

    virtual void resetZoom() = 0;

    virtual SearchResult& search(const QString &term, bool isRegEx) = 0;

    SearchResult& searchResult();

    virtual void setSearchSelection(const SearchResult::SearchEntry &result) = 0;

    virtual void setupView(QSharedPointer<AbstractModelInstance> modelInstance) = 0;

    QSharedPointer<AbstractViewConfiguration> viewConfig() const;

    void setViewConfig(QSharedPointer<AbstractViewConfiguration> viewConfig);

    virtual bool hasData() const = 0;

protected:
    virtual void updateIdentifierFilter();

    virtual void updateLabelFilter();

    virtual void updateValueFilter();

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
};

}
}
}

#endif // ABSTRACTVIEWFRAME_H
