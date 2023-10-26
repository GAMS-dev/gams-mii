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
#include "modelinspector.h"
#include "ui_modelinspector.h"
#include "modelinstance.h"
#include "sectiontreemodel.h"
#include "sectiontreeitem.h"
#include "viewconfigurationprovider.h"
#include "symbolviewframe.h"

#include <QtConcurrent>
#include <QDir>

#include <QDebug>

namespace gams {
namespace studio{
namespace mii {

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelInspector)
    , mSectionModel(new SectionTreeModel(this))
    , mModelInstance(new EmptyModelInstance)
{
    ui->setupUi(this);
    ui->bpScalingFrame->setViewId((int)ViewDataType::BP_Scaling);
    ui->bpOverviewFrame->setViewId((int)ViewDataType::BP_Overview);
    ui->bpCountFrame->setViewId((int)ViewDataType::BP_Count);
    ui->bpAverageFrame->setViewId((int)ViewDataType::BP_Average);
    mSectionModel->loadModelData(ui->stackedWidget);
    ui->sectionView->setModel(mSectionModel);
    loadModelInstance(false);
    setupConnections();
    ui->sectionView->expandAll();
    ui->splitter->setStretchFactor(1,1);
}

ModelInspector::~ModelInspector()
{
    delete ui;
}

QString ModelInspector::scratchDir() const
{
    return mScratchDir;
}

void ModelInspector::setScratchDir(const QString &scratchDir)
{
    mScratchDir = scratchDir;
}

QString ModelInspector::workspace() const
{
    return mWorkspace;
}

void ModelInspector::setWorkspace(const QString &workingDir)
{
    mWorkspace = workingDir;
}

QString ModelInspector::systemDirectory() const
{
    return mSystemDir;
}

void ModelInspector::setSystemDirectory(const QString &systemDir)
{
    mSystemDir = systemDir;
}

bool ModelInspector::showOutput() const
{
    return mModelInstance->useOutput();
}

void ModelInspector::setShowOutput(bool showOutput)
{
    mModelInstance->setUseOutput(showOutput);
}

void ModelInspector::setShowAbsoluteValues(bool absoluteValues)
{
    for (auto widget : mSectionModel->data()->widgets()) {
        widget->setShowAbsoluteValues(absoluteValues);
    }
}

SearchResult& ModelInspector::searchHeaders(const QString &term, bool isRegEx)
{
    auto frame = currentView();
    return frame ? frame->search(term, isRegEx) : mDefaultSearchResult;
}

SearchResult& ModelInspector::searchResult()
{
    auto frame = currentView();
    return frame ? frame->searchResult() : mDefaultSearchResult;
}

ViewActionStates ModelInspector::viewActionStates() const
{
    return ui->sectionView->viewActionStates();
}

void ModelInspector::loadModelInstance(bool loadModel)
{
    clearCustomViews();
    setupModelInstanceView(loadModel);
    emit newLogMessage(mModelInstance->logMessages());
}

void ModelInspector::reloadModelInstance()
{
    setupModelInstanceView(true);
    // TODO !!! circle.gms diffenrences?
    //ui->bpScalingFrame->setupView(mModelInstance);
    //ui->bpOverviewFrame->setupView(mModelInstance);
    //ui->bpCountFrame->setupView(mModelInstance);
    //ui->bpAverageFrame->setupView(mModelInstance);
    //ui->postoptFrame->setupView(mModelInstance);
    //for (auto view : qAsConst(mCustomViews)) {
    //    qDebug() << "lalala";
    //    view->setupView(mModelInstance);
    //}
    emit newLogMessage(mModelInstance->logMessages());
}

QSharedPointer<AbstractModelInstance> ModelInspector::modelInstance() const
{
    return mModelInstance;
}

IdentifierFilter ModelInspector::identifierFilter() const
{
    auto frame = currentView();
    if (frame) return frame->identifierFilter();
    return IdentifierFilter();
}

IdentifierFilter ModelInspector::defaultIdentifierFilter() const
{
    auto frame = currentView();
    if (frame) return frame->defaultIdentifierFilter();
    return IdentifierFilter();
}

void ModelInspector::setIdentifierFilter(const IdentifierFilter &filter)
{
    auto frame = currentView();
    if (frame) frame->setIdentifierFilter(filter);
}

ValueFilter ModelInspector::valueFilter() const
{
    auto frame = currentView();
    if (frame) return frame->valueFilter();
    return ValueFilter();
}

ValueFilter ModelInspector::defaultValueFilter() const
{
    auto frame = currentView();
    if (frame) return frame->defaultValueFilter();
    return ValueFilter();
}

void ModelInspector::setValueFilter(const ValueFilter &filter)
{
    auto frame = currentView();
    if (frame) frame->setValueFilter(filter);
}

LabelFilter ModelInspector::labelFilter() const
{
    auto frame = currentView();
    if (frame) return frame->labelFilter();
    return LabelFilter();
}

LabelFilter ModelInspector::defaultLabelFilter() const
{
    auto frame = currentView();
    if (frame) return frame->defaultLabelFilter();
    return LabelFilter();
}

void ModelInspector::setLabelFilter(const LabelFilter &filter)
{
    auto frame = currentView();
    if (frame) frame->setLabelFilter(filter);
}

Aggregation ModelInspector::aggregation() const
{
    auto frame = currentView();
    if (frame) return frame->currentAggregation();
    return Aggregation();
}

Aggregation ModelInspector::defaultAggregation() const
{
    auto frame = currentView();
    if (frame) return frame->defaultAggregation();
    return Aggregation();
}

void ModelInspector::setAggregation(const Aggregation &aggregation)
{
    auto frame = currentView();
    if (frame) frame->setAggregation(aggregation);
}

ViewDataType ModelInspector::viewType() const
{
    auto frame = currentView();
    if (frame) return frame->type();
    return ViewDataType::Unknown;
}

void ModelInspector::resetColumnRowFilter()
{
    auto frame = currentView();
    if (frame) frame->updateView();
}

void ModelInspector::cancelRun()
{
    if (mFutureData.isRunning()) {
        mFutureData.cancel();
        mFutureData.waitForFinished();
    }
}

void ModelInspector::zoomIn()
{
    for (auto widget : mSectionModel->data()->widgets()) {
        widget->zoomIn();
    }
}

void ModelInspector::zoomOut()
{
    for (auto widget : mSectionModel->data()->widgets()) {
        widget->zoomOut();
    }
}

void ModelInspector::resetZoom()
{
    for (auto widget : mSectionModel->data()->widgets()) {
        widget->resetZoom();
    }
}

void ModelInspector::saveModelView()
{
    auto view = currentView();
    if (!view) {
        emit newLogMessage("ERROR: ModelInspector::saveModelView() widget nullptr!");
        return;
    }
    auto text = ui->sectionView->currentIndex().data().toString();
    auto clone = view->clone(ViewConfigurationProvider::nextViewId());
    ui->stackedWidget->addWidget(clone);
    ViewDataType dataType = ViewDataType::Unknown;
    switch (clone->type()) {
    case ViewDataType::BP_Overview:
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
    case ViewDataType::BP_Scaling:
        dataType = ViewDataType::Blockpic;
        connect(static_cast<AbstractBPViewFrame*>(clone), &AbstractBPViewFrame::filtersChanged,
                this, &ModelInspector::filtersChanged);
        connect(static_cast<AbstractBPViewFrame*>(clone), &AbstractBPViewFrame::newSymbolViewRequested,
                this, &ModelInspector::createNewSymbolView);
        break;
    default:
        dataType = clone->type();
        break;
    }
    mSectionModel->appendCustomView(text, view->type(), clone);
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom, dataType);
}

void ModelInspector::createNewSymbolView()
{
    auto currentBPView = static_cast<BPScalingViewFrame*>(currentView());
    if (!currentBPView) {
        emit newLogMessage("ERROR: ModelInspector::createNewSymbolView() widget nullptr!");
        return;
    }
    auto view = new SymbolViewFrame(ViewConfigurationProvider::nextViewId(),
                                    mModelInstance,
                                    ui->stackedWidget,
                                    currentBPView->windowFlags());
    view->viewConfig()->currentValueFilter().UseAbsoluteValues =
            currentBPView->viewConfig()->currentValueFilter().UseAbsoluteValues;
    view->viewConfig()->currentValueFilter().UseAbsoluteValuesGlobal =
            currentBPView->viewConfig()->currentValueFilter().UseAbsoluteValuesGlobal;
    view->viewConfig()->updateIdentifierFilter(currentBPView->selectedEquations(),
                                               currentBPView->selectedVariables());
    view->setupView(mModelInstance);
    ui->stackedWidget->addWidget(view);
    QString pageName = Mi::SymbolView;
    if (!currentBPView->selectedEquations().isEmpty() && !currentBPView->selectedVariables().isEmpty()) {
        pageName = currentBPView->selectedEquations().constFirst()->name() + " + " +
                   currentBPView->selectedVariables().constFirst()->name();
    }
    mSectionModel->appendCustomView(pageName, ViewDataType::Symbols, view);
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom, ViewDataType::Symbols);
    connect(view, &SymbolViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    view->updateView();
}

void ModelInspector::removeModelView()
{
    auto currentIndex = ui->sectionView->currentIndex();
    auto item = static_cast<SectionTreeItem*>(currentIndex.internalPointer());
    auto parent = item->parent();
    for (auto widget : mSectionModel->removeItem(item))
    {
        ui->stackedWidget->removeWidget(widget);
        mModelInstance->remove(widget->viewId());
        delete widget;
    }
    if (!parent->childCount()) {
        mSectionModel->removeItem(parent);
    }
    auto customViewIndex = ui->sectionView->model()->index((int)ViewType::Custom, 0);
    if (!ui->sectionView->model()->rowCount(customViewIndex)) {
        auto predefinedViewIndex = ui->sectionView->model()->index((int)ViewType::Predefined, 0);
        int pos = ui->sectionView->model()->rowCount(predefinedViewIndex) - 1;
        auto index = ui->sectionView->model()->index(pos, 0, predefinedViewIndex);
        ui->sectionView->setCurrentIndex(index);
    } else if (ui->sectionView->model()->rowCount(customViewIndex) > 0) {
        auto group = ui->sectionView->model()->index(ui->sectionView->model()->rowCount(customViewIndex)-1, 0, customViewIndex);
        auto lastIndex = ui->sectionView->model()->index(ui->sectionView->model()->rowCount(group)-1, 0, group);
        ui->sectionView->setCurrentIndex(lastIndex);
    }
    ui->sectionView->expandAll();
}

void ModelInspector::setCurrentView()
{
    auto view = currentView();
    if (!view)
        return;
    int index = currentViewIndex(view);
    ui->stackedWidget->setCurrentIndex(index);
    if (!view->hasData() && mFutureData.isFinished()) {
        view->setupView(mModelInstance);
    }
    emit viewChanged((int)view->type());
}

void ModelInspector::setCurrentViewIndex(ViewType viewType, ViewDataType viewDataType)
{
    if (viewType != ViewType::Custom)
        return;
    auto customViewIndex = ui->sectionView->model()->index((int)viewType, 0);
    int pos = ui->sectionView->model()->rowCount(customViewIndex) - 1;
    if (pos < 0)
        return;
    QModelIndex index;
    auto groupParent = ui->sectionView->model()->index(pos, 0, customViewIndex).parent();
    for (int r=0; r<groupParent.model()->rowCount(groupParent); ++r) {
        auto groupIndex = ui->sectionView->model()->index(r, 0, groupParent);
        if (groupIndex.data(SectionTreeModel::ItemDataTypeRole).toInt() == (int)viewDataType) {
            int last = groupIndex.model()->rowCount(groupIndex)-1;
            index = ui->sectionView->model()->index(last, 0, groupIndex);
            break;
        }
    }
    if (index.isValid())
        ui->sectionView->setCurrentIndex(index);
}

void ModelInspector::setSearchSelection(const gams::studio::mii::SearchResult::SearchEntry &result)
{
    auto frame = currentView();
    if (frame) frame->setSearchSelection(result);
}

void ModelInspector::setupConnections()
{
    connect(ui->sectionView, &SectionTreeView::currentItemChanged,
            this, &ModelInspector::setCurrentView);
    connect(ui->bpScalingFrame, &BPScalingViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    connect(ui->sectionView, &SectionTreeView::saveViewTriggered,
            this, &ModelInspector::saveModelView);
    connect(ui->bpOverviewFrame, &AbstractBPViewFrame::newSymbolViewRequested,
            this, &ModelInspector::createNewSymbolView);
    connect(ui->bpCountFrame, &AbstractBPViewFrame::newSymbolViewRequested,
            this, &ModelInspector::createNewSymbolView);
    connect(ui->bpAverageFrame, &AbstractBPViewFrame::newSymbolViewRequested,
            this, &ModelInspector::createNewSymbolView);
    connect(ui->bpScalingFrame, &AbstractBPViewFrame::newSymbolViewRequested,
            this, &ModelInspector::createNewSymbolView);
    connect(ui->sectionView, &SectionTreeView::removeViewTriggered,
            this, &ModelInspector::removeModelView);
    connect(this, &ModelInspector::dataLoaded,
            this, &ModelInspector::selectScalingView);
}

void ModelInspector::setupModelInstanceView(bool loadModel)
{
    ui->bpOverviewFrame->setupView(QSharedPointer<AbstractModelInstance>(new EmptyModelInstance));
    ui->bpCountFrame->setupView(QSharedPointer<AbstractModelInstance>(new EmptyModelInstance));
    ui->bpAverageFrame->setupView(QSharedPointer<AbstractModelInstance>(new EmptyModelInstance));
    ui->postoptFrame->setupView(QSharedPointer<AbstractModelInstance>(new EmptyModelInstance));
    auto loadData = [this, loadModel]{
        bool useOutput = mModelInstance->useOutput();
        if (loadModel) {
            mModelInstance = QSharedPointer<AbstractModelInstance>(new ModelInstance(useOutput,
                                                                                     mWorkspace,
                                                                                     mSystemDir,
                                                                                     mScratchDir));
            if (mModelInstance->state() == AbstractModelInstance::Error)
                emit newLogMessage(mModelInstance->logMessages());
        }
        if (mModelInstance->state() == AbstractModelInstance::Error) {
            mModelInstance = QSharedPointer<AbstractModelInstance>(new EmptyModelInstance);
            mModelInstance->setUseOutput(useOutput);
        }

        mModelInstance->loadBaseData();
        emit dataLoaded();
    };
    mFutureData = QtConcurrent::run(loadData);
}

void ModelInspector::selectScalingView()
{
    ui->bpScalingFrame->setupView(mModelInstance);
    auto root = ui->sectionView->model()->index((int)ViewType::Predefined, 0);
    auto group = ui->sectionView->model()->index(0, 0, root);
    auto index = ui->sectionView->model()->index((int)ViewDataType::BP_Scaling, 0, group);
    ui->sectionView->setCurrentIndex(index);
}

void ModelInspector::clearCustomViews()
{
    auto customViewIndex = ui->sectionView->model()->index((int)ViewType::Custom, 0);
    int rows = ui->sectionView->model()->rowCount(customViewIndex);
    ui->sectionView->model()->removeRows(0, rows, customViewIndex);
    for (auto widget : mSectionModel->removeCustomRows()) {
        ui->stackedWidget->removeWidget(widget);
        widget->setParent(nullptr);
        mModelInstance->remove(widget->viewId());
        delete widget;
    }
}

AbstractViewFrame* ModelInspector::currentView() const
{// TODO !!! template for different frame types?
    auto currentIndex = ui->sectionView->currentIndex();
    if (!currentIndex.isValid())
        return nullptr;
    auto item = static_cast<SectionTreeItem*>(currentIndex.internalPointer());
    return item ? item->widget() : nullptr;
}

int ModelInspector::currentViewIndex(AbstractViewFrame* view) const
{
    if (view->parentWidget() == ui->stackedWidget) {
        return ui->stackedWidget->indexOf(view);
    }
    return ui->stackedWidget->indexOf(view->parentWidget());
}

}
}
}
