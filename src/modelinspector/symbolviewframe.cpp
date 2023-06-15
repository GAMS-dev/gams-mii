#include "symbolviewframe.h"
#include "modelinspector/columnrowfiltermodel.h"
#include "modelinspector/identifierfiltermodel.h"
#include "modelinspector/labelfiltermodel.h"
#include "viewconfigurationprovider.h"
#include "hierarchicalheaderview.h"
#include "ui_standardtableviewframe.h"
#include "symbolmodelinstancetablemodel.h"
#include "abstractmodelinstance.h"
#include "valueformatproxymodel.h"

namespace gams {
namespace studio{
namespace modelinspector {

SymbolViewFrame::SymbolViewFrame(int view,
                                 QSharedPointer<AbstractModelInstance> modelInstance,
                                 QWidget *parent,
                                 Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mViewConfig = QSharedPointer<AbstractViewConfiguration>(ViewConfigurationProvider::configuration(type(), modelInstance));
    mViewConfig->setView(view);
}

SymbolViewFrame::SymbolViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                                 QSharedPointer<AbstractViewConfiguration> viewConfig,
                                 QWidget *parent, Qt::WindowFlags f)
    : AbstractStandardTableViewFrame(parent, f)
{
    mModelInstance = modelInstance;
    mViewConfig = viewConfig;
}

AbstractTableViewFrame *SymbolViewFrame::clone(int view)
{
    auto viewConfig = QSharedPointer<AbstractViewConfiguration>(mModelInstance->clone(this->view(), view));
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
    mModelInstance->loadData(mViewConfig);
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
    mViewConfig->setCurrentValueFilter(filter);
    if (mViewConfig->currentAggregation().useAbsoluteValues() != mViewConfig->currentValueFilter().UseAbsoluteValues) {
        setShowAbsoluteValues(mViewConfig->currentValueFilter().UseAbsoluteValues);
    } else if (mValueFormatModel) {
        mValueFormatModel->setValueFilter(filter);
    }
}

void SymbolViewFrame::updateView()
{
    mModelInstanceModel->setModelInstance(mModelInstance);
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void SymbolViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    if (mModelInstanceModel && mViewConfig->currentValueFilter().UseAbsoluteValues != absoluteValues) {
        mViewConfig->currentAggregation().setUseAbsoluteValues(absoluteValues);
        mViewConfig->currentValueFilter().UseAbsoluteValues = absoluteValues;
        mModelInstance->loadData(mViewConfig);
        emit mModelInstanceModel->dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole});
        mValueFormatModel->setValueFilter(mViewConfig->currentValueFilter());
    }
}

void SymbolViewFrame::setView(int view)
{
    mViewConfig->setView(view);
    mModelInstanceModel->setView(view);
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
    mHorizontalHeader->setView(mViewConfig->view());
    connect(mHorizontalHeader, &HierarchicalHeaderView::filterChanged,
            this, &SymbolViewFrame::setIdentifierLabelFilter);
    mVerticalHeader = new HierarchicalHeaderView(Qt::Vertical,
                                                 mModelInstance,
                                                 ui->tableView);
    mVerticalHeader->setViewType(type());
    mVerticalHeader->setView(mViewConfig->view());
    connect(mVerticalHeader, &HierarchicalHeaderView::filterChanged,
            this, &SymbolViewFrame::setIdentifierLabelFilter);

    auto baseModel = new SymbolModelInstanceTableModel(mModelInstance, ui->tableView);
    baseModel->setView(mViewConfig->view());
    mValueFormatModel = new JaccobianValueFormatProxyModel(ui->tableView);
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

    mModelInstanceModel = QSharedPointer<SymbolModelInstanceTableModel>(baseModel);

    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
}

}
}
}
