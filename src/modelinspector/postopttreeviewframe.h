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
#ifndef POSTOPTTREEVIEWFRAME_H
#define POSTOPTTREEVIEWFRAME_H

#include "abstractviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class PostoptTreeViewFrame;
}


class PostoptTreeViewFrame final : public AbstractViewFrame
{
    Q_OBJECT

public:
    PostoptTreeViewFrame(QWidget *parent = nullptr,
                         Qt::WindowFlags f = Qt::WindowFlags());

    PostoptTreeViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig,
                         QWidget *parent = nullptr,
                         Qt::WindowFlags f = Qt::WindowFlags());

    ~PostoptTreeViewFrame();

    AbstractViewFrame* clone(int view) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setAggregation(const Aggregation &aggregation) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    SearchResult& search(const QString &term, bool isRegEx) override;

    void setSearchSelection(const SearchResult::SearchEntry &result) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    ViewDataType type() const override;

    void reset() override;

    void updateView() override;

    void zoomIn() override;

    void zoomOut() override;

    void resetZoom() override;

private:
    void setupView();

protected:
    Ui::PostoptTreeViewFrame* ui;
};

}
}
}

#endif // POSTOPTTREEVIEWFRAME_H
