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
    ui->bpScalingFrame->setView((int)ViewDataType::BP_Scaling);
    ui->bpOverviewFrame->setView((int)ViewDataType::BP_Overview);
    ui->bpCountFrame->setView((int)ViewDataType::BP_Count);
    ui->bpAverageFrame->setView((int)ViewDataType::BP_Average);
    mSectionModel->loadModelData();
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
    ui->bpScalingFrame->setShowAbsoluteValues(absoluteValues);
    ui->bpAverageFrame->setShowAbsoluteValues(absoluteValues);
    ui->bpCountFrame->setShowAbsoluteValues(absoluteValues);
    ui->postoptFrame->setShowAbsoluteValues(absoluteValues);
    for (auto view : std::as_const(mCustomViews)) {
        view->setShowAbsoluteValues(absoluteValues);
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
    ui->bpOverviewFrame->zoomIn();
    ui->bpCountFrame->zoomIn();
    ui->bpAverageFrame->zoomIn();
    ui->bpScalingFrame->zoomIn();
    ui->postoptFrame->zoomIn();
    for (auto view : qAsConst(mCustomViews)) {
        view->zoomIn();
    }
}

void ModelInspector::zoomOut()
{
    ui->bpOverviewFrame->zoomOut();
    ui->bpCountFrame->zoomOut();
    ui->bpAverageFrame->zoomOut();
    ui->bpScalingFrame->zoomOut();
    ui->postoptFrame->zoomOut();
    for (auto view : qAsConst(mCustomViews)) {
        view->zoomOut();
    }
}

void ModelInspector::resetZoom()
{
    ui->bpOverviewFrame->resetZoom();
    ui->bpCountFrame->resetZoom();
    ui->bpAverageFrame->resetZoom();
    ui->bpScalingFrame->resetZoom();
    ui->postoptFrame->resetZoom();
    for (auto view : qAsConst(mCustomViews)) {
        view->resetZoom();
    }
}

void ModelInspector::saveModelView()
{
    auto view = currentView();
    if (!view) return;
    auto text = ui->sectionView->currentIndex().data().toString();
    auto clone = view->clone(ui->stackedWidget->count());
    clone->setParent(ui->stackedWidget, view->windowFlags());
    int page = ui->stackedWidget->addWidget(clone);
    mCustomViews[page] = clone;
    switch (clone->type()) {
    case ViewDataType::BP_Overview:
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
    case ViewDataType::BP_Scaling:
        connect(static_cast<AbstractBPViewFrame*>(clone), &AbstractBPViewFrame::filtersChanged,
                this, &ModelInspector::filtersChanged);
        connect(static_cast<AbstractBPViewFrame*>(clone), &AbstractBPViewFrame::newSymbolViewRequested,
                this, &ModelInspector::createNewSymbolView);
        break;
    default:
        break;
    }
    mSectionModel->appendCustomView(text, view->type(), page);
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom);
}

void ModelInspector::createNewSymbolView()
{
    auto currentMinMax = static_cast<BPScalingViewFrame*>(currentView());
    auto view = new SymbolViewFrame(ui->stackedWidget->count(), mModelInstance,
                                    ui->stackedWidget, currentMinMax->windowFlags());
    view->viewConfig()->currentValueFilter().UseAbsoluteValues =
            currentMinMax->viewConfig()->currentValueFilter().UseAbsoluteValues;
    view->viewConfig()->currentValueFilter().UseAbsoluteValuesGlobal =
            currentMinMax->viewConfig()->currentValueFilter().UseAbsoluteValuesGlobal;
    view->viewConfig()->updateIdentifierFilter(currentMinMax->selectedEquations(),
                                               currentMinMax->selectedVariables());
    view->setupView(mModelInstance);
    int page = ui->stackedWidget->addWidget(view);
    mCustomViews[page] = view;
    QString pageName = "Symbol View";
    if (!currentMinMax->selectedEquations().isEmpty() && !currentMinMax->selectedVariables().isEmpty()) {
        pageName = currentMinMax->selectedEquations().constFirst()->name() + " + " +
                   currentMinMax->selectedVariables().constFirst()->name();
    }
    mSectionModel->appendCustomView(pageName, ViewDataType::Symbols, page);
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom);
    connect(view, &SymbolViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    view->updateView();
}

void ModelInspector::removeModelView()
{
    auto customViewIndex = ui->sectionView->model()->index((int)ViewType::Custom, 0);
    auto currentIndex = ui->sectionView->currentIndex();
    if (customViewIndex != currentIndex.parent()) return;
    auto item = static_cast<SectionTreeItem*>(currentIndex.internalPointer());

    ui->sectionView->model()->removeRows(currentIndex.row(), 1, customViewIndex);
    if (mCustomViews.contains(item->page())) {
        auto page = mCustomViews.take(item->page());
        if (page) {
            ui->stackedWidget->removeWidget(page);
            page->setParent(nullptr);
            delete page;
        }
    }

    if (!ui->sectionView->model()->rowCount(customViewIndex)) {
        setCurrentViewIndex(ViewType::Predefined);
    } else if (ui->sectionView->model()->rowCount(customViewIndex) == 1) {
        auto lastIndex = ui->sectionView->model()->index(0, 0, customViewIndex);
        ui->sectionView->setCurrentIndex(lastIndex);
    }
}

void ModelInspector::setCurrentView(int index)
{
    Q_UNUSED(index);
    int page;
    auto currentIndex = ui->sectionView->currentIndex();
    if (!currentIndex.isValid())
        return;

    auto item = static_cast<SectionTreeItem*>(currentIndex.internalPointer());
    if (currentIndex.parent() != ui->sectionView->model()->index((int)ViewType::Custom, 0)) {
        if (item->page() < 0 || item->page() >= ui->stackedWidget->count())
            return;
        page = item->page();
    } else {
        auto wgt = mCustomViews[item->page()];
        if (!wgt)
            return;
        page = ui->stackedWidget->indexOf(wgt);
    }
    ui->stackedWidget->setCurrentIndex(page);
    if (!currentView()->hasData() && mFutureData.isFinished()) {
        currentView()->setupView(mModelInstance);
    }
    emit viewChanged((int)item->type());
}

void ModelInspector::setCurrentViewIndex(gams::studio::mii::ViewType type)
{
    auto customViewIndex = ui->sectionView->model()->index((int)type, 0);
    int pos = ui->sectionView->model()->rowCount(customViewIndex) - 1;
    auto index = ui->sectionView->model()->index(pos, 0, customViewIndex);
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
            this, &ModelInspector::updateView);
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

        mModelInstance->loadData();
        emit dataLoaded();
    };
    mFutureData = QtConcurrent::run(loadData);
}

void ModelInspector::updateView()
{
    ui->bpScalingFrame->setupView(mModelInstance);
    auto root = ui->sectionView->model()->index((int)ViewType::Predefined, 0);
    auto index = ui->sectionView->model()->index((int)ViewDataType::BP_Scaling, 0, root);
    ui->sectionView->setCurrentIndex(index);
}

void ModelInspector::clearCustomViews()
{
    auto customViewIndex = ui->sectionView->model()->index((int)ViewType::Custom, 0);
    int rows = ui->sectionView->model()->rowCount(customViewIndex);
    ui->sectionView->model()->removeRows(0, rows, customViewIndex);
    for (auto wgt : qAsConst(mCustomViews)) {
        ui->stackedWidget->removeWidget(wgt);
        wgt->setParent(nullptr);
        delete wgt;
    }
    mCustomViews.clear();
}

AbstractViewFrame* ModelInspector::currentView() const
{
    switch (ui->stackedWidget->currentIndex()) {
    case (int)ViewDataType::BP_Overview:
        return ui->bpOverviewFrame;
    case (int)ViewDataType::BP_Count:
        return ui->bpCountFrame;
    case (int)ViewDataType::BP_Average:
        return ui->bpAverageFrame;
    case (int)ViewDataType::BP_Scaling:
        return ui->bpScalingFrame;
    case (int)ViewDataType::Postopt:
        return ui->postoptFrame;
    default:
        if (mCustomViews.contains(ui->stackedWidget->currentIndex()))
            return mCustomViews[ui->stackedWidget->currentIndex()];
        return nullptr;
    }
}

}
}
}
