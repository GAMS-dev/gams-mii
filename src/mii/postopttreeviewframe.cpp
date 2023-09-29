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
#include "postopttreeviewframe.h"
#include "ui_postopttreeviewframe.h"
#include "postopttreemodel.h"
#include "viewconfigurationprovider.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace mii {

PostoptTreeViewFrame::PostoptTreeViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractViewFrame(parent, f)
    , ui(new Ui::PostoptTreeViewFrame)
{
    ui->setupUi(this);
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::defaultConfiguration());
}

PostoptTreeViewFrame::PostoptTreeViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                           QSharedPointer<AbstractViewConfiguration> viewConfig,
                                           QWidget *parent,
                                           Qt::WindowFlags f)
    : PostoptTreeViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    mViewConfig = viewConfig;
}

PostoptTreeViewFrame::~PostoptTreeViewFrame()
{

}

AbstractViewFrame *PostoptTreeViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
    if (!viewConfig)
        viewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(),
                                                                                                        mModelInstance));
    auto frame = new PostoptTreeViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    return frame;
}

void PostoptTreeViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    Q_UNUSED(filter);
}

void PostoptTreeViewFrame::setAggregation(const Aggregation &aggregation)
{
    Q_UNUSED(aggregation);
}

void PostoptTreeViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mViewConfig->currentValueFilter().isAbsolute() != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
        mModelInstance->loadData(mViewConfig);
        setupView();
    }
}

SearchResult &PostoptTreeViewFrame::search(const QString &term, bool isRegEx)
{
    Q_UNUSED(term);
    Q_UNUSED(isRegEx);
    return searchResult();
}

void PostoptTreeViewFrame::setSearchSelection(const SearchResult::SearchEntry &result)
{
    Q_UNUSED(result);
}

void PostoptTreeViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(), mModelInstance));
    mModelInstance->loadData(mViewConfig);
    setupView();
}

ViewDataType PostoptTreeViewFrame::type() const
{
    return ViewDataType::Postopt;
}

void PostoptTreeViewFrame::reset()
{

}

void PostoptTreeViewFrame::updateView()
{

}

void PostoptTreeViewFrame::zoomIn()
{
    ui->treeView->zoomIn(Mi::ZoomFactor);
}

void PostoptTreeViewFrame::zoomOut()
{
    ui->treeView->zoomOut(Mi::ZoomFactor);
}

void PostoptTreeViewFrame::resetZoom()
{
    ui->treeView->resetZoom();
}

bool PostoptTreeViewFrame::hasData() const
{
    return mBaseModel && mBaseModel->rowCount();
}

void PostoptTreeViewFrame::setupView()
{
    mBaseModel = new PostoptTreeModel(mViewConfig->view(),
                                      mModelInstance,
                                      ui->treeView);
    ui->treeView->setModel(mBaseModel);

    //auto oldSelectionModel = ui->treeView->selectionModel();
    //ui->treeView->setModel(baseModel);
    //delete oldSelectionModel;
    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
}

}
}
}
