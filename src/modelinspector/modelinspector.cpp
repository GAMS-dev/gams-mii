/*
 * This file is part of the GAMS Studio project.
 *
 * Copyright (c) 2017-2019 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2017-2019 GAMS Development Corp. <support@gams.com>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "modelinspector.h"
#include "ui_modelinspector.h"
#include "modelinstance.h"
#include "sectiontreemodel.h"
#include "sectiontreeitem.h"

#include <QDir>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelInspector)
    , mSectionModel(new SectionTreeModel)
{
    ui->setupUi(this);
    mSectionModel->loadModelData();
    ui->sectionView->setModel(mSectionModel);
    loadModelInstance(false);
    setupConnections();
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Predefined);
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
    QDir dir(mWorkspace + "/" + mScratchDir);
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());
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
    auto frame = currentView();
    if (frame) frame->setShowAbsoluteValues(absoluteValues);
}

QList<SearchResult> ModelInspector::searchHeaders(const QString &term,
                                                  bool isRegEx)
{
    auto frame = currentView();
    return frame ? frame->search(term, isRegEx) : QList<SearchResult>();
}

ViewActionStates ModelInspector::viewActionStates() const
{
    return ui->sectionView->viewActionStates();
}

void ModelInspector::loadModelInstance(bool loadModel)
{
    setupModelInstanceView(loadModel);
    ui->statisticEdit->showStatistic(mModelInstance);
    emit newLogMessage(mModelInstance->logMessages());
}

void ModelInspector::reloadModelInstance()
{
    loadModelInstance();
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
    if (frame) return frame->aggregation();
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
    if (frame) frame->setAggregation(aggregation, ui->stackedWidget->currentIndex());
}

PredefinedViewEnum ModelInspector::viewType() const
{
    auto frame = currentView();
    if (frame) return frame->type();
    return PredefinedViewEnum::Unknown;
}

void ModelInspector::resetColumnRowFilter()
{
    auto frame = currentView();
    if (frame) frame->updateView();
}

void ModelInspector::zoomIn()
{
    if (qApp->focusWidget() == ui->statisticEdit) {
        ui->statisticEdit->zoomIn();
    } else {
        auto frame = currentView();
        if (frame) frame->zoomIn();
    }
}

void ModelInspector::zoomOut()
{
    if (qApp->focusWidget() == ui->statisticEdit) {
        ui->statisticEdit->zoomOut();
    } else {
        auto frame = currentView();
        if (frame) frame->zoomOut();
    }
}

void ModelInspector::resetZoom()
{
    if (qApp->focusWidget() == ui->statisticEdit) {
        ui->statisticEdit->resetZoom();
    } else {
        auto frame = currentView();
        if (frame) frame->resetZoom();
    }
}

void ModelInspector::resetDefaultViews()
{
    ui->statisticEdit->resetZoom();
    ui->eqnAttrFrame->reset(PredefinedViewEnum::EqnAttributes);
    ui->eqnAttrFrame->resetZoom();
    ui->varAttrFrame->reset(PredefinedViewEnum::VarAttributes);
    ui->varAttrFrame->resetZoom();
    ui->jaccFrame->reset(PredefinedViewEnum::Jaccobian);
    ui->jaccFrame->resetZoom();
    ui->fullFrame->reset(PredefinedViewEnum::Full);
    ui->fullFrame->resetZoom();
    ui->minMaxFrame->reset(PredefinedViewEnum::MinMax);
    ui->minMaxFrame->resetZoom();
    Q_FOREACH(auto view, mCustomViews) {
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
    mSectionModel->appendCustomView(text, view->type(), page);
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom);
}

void ModelInspector::saveReducedModelView(gams::studio::modelinspector::PredefinedViewEnum type)
{
    auto view = currentView();
    if (!view) return;
    auto clone = ui->jaccFrame->clone(ui->stackedWidget->indexOf(ui->jaccPage));
    clone->setParent(ui->stackedWidget, view->windowFlags());
    clone->setIdentifierFilter(clone->defaultIdentifierFilter());
    int page = ui->stackedWidget->addWidget(clone);
    mCustomViews[page] = clone;
    if (type == PredefinedViewEnum::SymbolEqnView) {
        mSectionModel->appendCustomView(SymbolEqnView, view->type(), page);
    } else if (type == PredefinedViewEnum::SymbolVarView) {
        mSectionModel->appendCustomView(SymbolVarView, view->type(), page);
    } else {
        mSectionModel->appendCustomView(SymbolView, view->type(), page);
    }
    ui->sectionView->expandAll();
    setCurrentViewIndex(ViewType::Custom);
    auto minMaxView = dynamic_cast<MinMaxTableViewFrame*>(view);
    if (minMaxView) {
        auto newFilter = newIdentifierFilter(clone->identifierFilter(),
                                             minMaxView->selectedEquations(),
                                             minMaxView->selectedVariables());
        clone->setIdentifierFilter(newFilter);
    }
    clone->updateView();
    emit filtersChanged();
}

IdentifierFilter ModelInspector::newIdentifierFilter(const IdentifierFilter &currentFilter,
                                                     const QList<Symbol> &eqnFilter,
                                                     const QList<Symbol> &varFilter)
{// TODO performance
    IdentifierFilter newFilter;
    Q_FOREACH(auto state, currentFilter[Qt::Vertical]) {
        if (state.SymbolIndex < PredefinedHeaderLength)
            continue;
        Q_FOREACH(auto symbol, eqnFilter) {
            if (state.Text == symbol.name()) {
                state.Checked = Qt::Checked;
                break;
            } else {
                state.Checked = Qt::Unchecked;
            }
        }
        newFilter[Qt::Vertical][state.SymbolIndex] = state;
    }
    Q_FOREACH(auto state, currentFilter[Qt::Horizontal]) {
        if (state.SymbolIndex < PredefinedHeaderLength)
            continue;
        Q_FOREACH(auto symbol, varFilter) {
            if (state.Text == symbol.name()) {
                state.Checked = Qt::Checked;
                break;
            } else {
                state.Checked = Qt::Unchecked;
            }
        }
        newFilter[Qt::Horizontal][state.SymbolIndex] = state;
    }
    return newFilter;
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
    if (!currentIndex.isValid()) return;
    auto item = static_cast<SectionTreeItem*>(currentIndex.internalPointer());
    if (currentIndex.parent() != ui->sectionView->model()->index((int)ViewType::Custom, 0)) {
        if (item->page() < 0 || item->page() >= ui->stackedWidget->count()) return;
        page = item->page();
    } else {
        auto wgt = mCustomViews[item->page()];
        if (!wgt) return;
        page = ui->stackedWidget->indexOf(wgt);
    }
    ui->stackedWidget->setCurrentIndex(page);
    emit viewChanged((int)item->type());
}

void ModelInspector::setCurrentViewIndex(gams::studio::modelinspector::ViewType type)
{
    auto customViewIndex = ui->sectionView->model()->index((int)type, 0);
    int pos = ui->sectionView->model()->rowCount(customViewIndex) - 1;
    auto index = ui->sectionView->model()->index(pos, 0, customViewIndex);
    ui->sectionView->setCurrentIndex(index);
}

void ModelInspector::setSearchSelection(const gams::studio::modelinspector::SearchResult &result)
{
    auto frame = currentView();
    if (frame) frame->setSearchSelection(result);
}

void ModelInspector::setupConnections()
{
    connect(ui->sectionView, &SectionTreeView::currentItemChanged,
            this, &ModelInspector::setCurrentView);
    connect(ui->eqnAttrFrame, &EqnTableViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    connect(ui->varAttrFrame, &VarTableViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    connect(ui->fullFrame, &FullTableViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    connect(ui->minMaxFrame, &MinMaxTableViewFrame::filtersChanged,
            this, &ModelInspector::filtersChanged);
    connect(ui->sectionView, &SectionTreeView::saveViewTriggered,
            this, &ModelInspector::saveModelView);
    connect(ui->minMaxFrame, &MinMaxTableViewFrame::newModelView,
            this, &ModelInspector::saveReducedModelView);
    connect(ui->sectionView, &SectionTreeView::removeViewTriggered,
            this, &ModelInspector::removeModelView);
}

void ModelInspector::setupModelInstanceView(bool loadModel)
{
    clearCustomViews();
    if (loadModel) {
        bool showOutput = mModelInstance->useOutput();
        mModelInstance = QSharedPointer<AbstractModelInstance>(new ModelInstance(mWorkspace,
                                                                                 mSystemDir,
                                                                                 mScratchDir));
        mModelInstance->setUseOutput(showOutput);
    } else {
        mModelInstance = QSharedPointer<AbstractModelInstance>(new EmptyModelInstance);
    }
    mModelInstance->initialize();
    LabelFilter labelFilter;
    mModelInstance->loadData(labelFilter);

    ui->eqnAttrFrame->setupView(mModelInstance, (int)PredefinedViewEnum::EqnAttributes);
    ui->varAttrFrame->setupView(mModelInstance, (int)PredefinedViewEnum::VarAttributes);
    ui->jaccFrame->setupView(mModelInstance, (int)PredefinedViewEnum::Jaccobian);
    ui->fullFrame->setupView(mModelInstance, (int)PredefinedViewEnum::Full);
    ui->minMaxFrame->setupView(mModelInstance, (int)PredefinedViewEnum::MinMax);

    ui->eqnAttrFrame->setupFiltersAggregation(ui->fullFrame->model(), labelFilter);
    ui->varAttrFrame->setupFiltersAggregation(ui->fullFrame->model(), labelFilter);
    ui->jaccFrame->setupFiltersAggregation(ui->fullFrame->model(), labelFilter);
    ui->fullFrame->setupFiltersAggregation(ui->fullFrame->model(), labelFilter);
    ui->minMaxFrame->setupFiltersAggregation(ui->fullFrame->model(), labelFilter);
    ui->minMaxFrame->setAggregation(ui->minMaxFrame->defaultAggregation(),
                                    ui->stackedWidget->indexOf(ui->minMaxPage));

    setCurrentViewIndex(ViewType::Predefined);
}

void ModelInspector::clearCustomViews()
{
    auto customViewIndex = ui->sectionView->model()->index((int)ViewType::Custom, 0);
    int rows = ui->sectionView->model()->rowCount(customViewIndex);
    ui->sectionView->model()->removeRows(0, rows, customViewIndex);
    Q_FOREACH(auto wgt, mCustomViews) {
        ui->stackedWidget->removeWidget(wgt);
        wgt->setParent(nullptr);
        delete wgt;
    }
    mCustomViews.clear();
}

TableViewFrame* ModelInspector::currentView() const
{
    switch (ui->stackedWidget->currentIndex()) {
    case (int)PredefinedViewEnum::EqnAttributes:
        return ui->eqnAttrFrame;
    case (int)PredefinedViewEnum::VarAttributes:
        return ui->varAttrFrame;
    case (int)PredefinedViewEnum::Jaccobian:
        return ui->jaccFrame;
    case (int)PredefinedViewEnum::Full:
        return ui->fullFrame;
    case (int)PredefinedViewEnum::MinMax:
        return ui->minMaxFrame;
    default:
        if (mCustomViews.contains(ui->stackedWidget->currentIndex()))
            return mCustomViews[ui->stackedWidget->currentIndex()];
        return nullptr;
    }
}

}
}
}
