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
#include "symbolviewframe.h"
#include "mii/identifierfiltermodel.h"
#include "mii/labelfiltermodel.h"
#include "viewconfigurationprovider.h"
#include "hierarchicalheaderview.h"
#include "ui_standardtableviewframe.h"
#include "symbolmodelinstancetablemodel.h"
#include "abstractmodelinstance.h"
#include "valueformatproxymodel.h"

namespace gams {
namespace studio{
namespace mii {

SymbolViewFrame::SymbolViewFrame(int view,
                                 QSharedPointer<AbstractModelInstance> modelInstance,
                                 QWidget *parent,
                                 Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(), modelInstance));
    mViewConfig->setViewId(view);
}

SymbolViewFrame::SymbolViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                 QSharedPointer<AbstractViewConfiguration> viewConfig,
                                 QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *SymbolViewFrame::clone(int viewId)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->viewId(), viewId));
    auto frame = new SymbolViewFrame(mModelInstance, viewConfig, parentWidget(), windowFlags());
    frame->setupView();
    frame->setValueFilter(frame->viewConfig()->currentValueFilter());
    frame->setLabelFilter(frame->viewConfig()->currentLabelFiler());
    frame->setIdentifierFilter(frame->viewConfig()->currentIdentifierFilter());
    for (auto iter=mViewConfig->currentIdentifierFilter()[Qt::Horizontal].constBegin();
         iter!=mViewConfig->currentIdentifierFilter()[Qt::Horizontal].constEnd(); ++iter) {
        frame->mIdentifierLabelFilterModel->setIdentifierState(*iter, Qt::Horizontal);
        frame->mHorizontalHeader->setIdentifierState(*iter);
    }
    for (auto iter=mViewConfig->currentIdentifierFilter()[Qt::Vertical].constBegin();
         iter!=mViewConfig->currentIdentifierFilter()[Qt::Vertical].constEnd(); ++iter) {
        frame->mIdentifierLabelFilterModel->setIdentifierState(*iter, Qt::Vertical);
        frame->mVerticalHeader->setIdentifierState(*iter);
    }
    return frame;
}

void SymbolViewFrame::setupView(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mViewConfig->setModelInstance(mModelInstance);
    mModelInstance->loadViewData(mViewConfig);
    setupView();
}

ViewDataType SymbolViewFrame::type() const
{
    return ViewDataType::Symbols;
}

void SymbolViewFrame::setLabelFilter(const LabelFilter &filter)
{
    AbstractStandardTableViewFrame::setLabelFilter(filter);
    if (mLabelFilterModel)
        mLabelFilterModel->setLabelFilter(filter);
    if (mHorizontalHeader)
        mHorizontalHeader->resetSymbolLabelFilters();
    if (mVerticalHeader)
        mVerticalHeader->resetSymbolLabelFilters();
    if (mIdentifierLabelFilterModel) {
        mIdentifierLabelFilterModel->clearIdentifierFilter();
        mIdentifierLabelFilterModel->invalidate();
    }
}

void SymbolViewFrame::setValueFilter(const ValueFilter &filter)
{
    if (!mBaseModel)
        return;
    if (filter.Reset) {
        mViewConfig->setCurrentValueFilter(filter);
        mModelInstance->loadViewData(mViewConfig);
    } else if (mViewConfig->currentValueFilter().UseAbsoluteValues != filter.UseAbsoluteValues) {
        mViewConfig->currentValueFilter().UseAbsoluteValues = filter.UseAbsoluteValues;
        mModelInstance->loadViewData(mViewConfig);
    }
    if (!filter.Reset) {
        mViewConfig->setCurrentValueFilter(filter);
    }
    emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
    mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
}

void SymbolViewFrame::updateView()
{
    mBaseModel->setModelInstance(mModelInstance);
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

bool SymbolViewFrame::hasData() const
{
    return mBaseModel && mBaseModel->rowCount() && mBaseModel->columnCount();
}

void SymbolViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (!mBaseModel)
        return;
    mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
    mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
    mModelInstance->loadViewData(mViewConfig);
    emit mBaseModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
    mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
}

void SymbolViewFrame::setViewId(int view)
{
    mViewConfig->setViewId(view);
    mBaseModel->setView(view);
}

void SymbolViewFrame::setIdentifierLabelFilter(const IdentifierState &state,
                                               Qt::Orientation orientation)
{
    if (!mIdentifierLabelFilterModel) {
        return;
    }
    if (state.disabled() && mIdentifierFilterModel) {
        setIdentifierFilterCheckState(state.SymbolIndex, Qt::Unchecked, orientation);
        mIdentifierFilterModel->setIdentifierFilter(mViewConfig->currentIdentifierFilter());
    } else {
        mViewConfig->currentIdentifierFilter()[orientation][state.SymbolIndex] = state;
        mIdentifierLabelFilterModel->setIdentifierState(state, orientation);
    }
    updateView();
    emit filtersChanged();
}

void SymbolViewFrame::setupView()
{
    mHorizontalHeader = new HierarchicalHeaderView(Qt::Horizontal,
                                                   mModelInstance,
                                                   ui->tableView);
    mHorizontalHeader->setViewType(type());
    mHorizontalHeader->setView(mViewConfig->viewId());
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &SymbolViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(type());
    mVerticalHeader->setView(mViewConfig->viewId());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &SymbolViewFrame::setIdentifierLabelFilter);

    auto baseModel = new SymbolModelInstanceTableModel(mModelInstance, ui->tableView);
    baseModel->setView(mViewConfig->viewId());
    mValueFormatModel = new JacobianValueFormatProxyModel(ui->tableView);
    mValueFormatModel->setSourceModel(baseModel);
    mLabelFilterModel = new LabelFilterModel(mModelInstance, ui->tableView);
    mLabelFilterModel->setSourceModel(mValueFormatModel);
    mIdentifierFilterModel = new IdentifierFilterModel(mModelInstance, ui->tableView);
    mIdentifierFilterModel->setSourceModel(mLabelFilterModel);
    mIdentifierLabelFilterModel = new IdentifierLabelFilterModel(mModelInstance, ui->tableView);
    mIdentifierLabelFilterModel->setSourceModel(mIdentifierFilterModel);

    ui->tableView->setHorizontalHeader(mHorizontalHeader);
    ui->tableView->setVerticalHeader(mVerticalHeader);
    auto oldSelectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(mIdentifierLabelFilterModel);
    delete oldSelectionModel;
    mHorizontalHeader->setVisible(true);
    mVerticalHeader->setVisible(true);

    mBaseModel = QSharedPointer<SymbolModelInstanceTableModel>(baseModel);

    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
}

}
}
}
