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
#ifndef MODELINSPECTOR_H
#define MODELINSPECTOR_H

#include <QFuture>
#include <QSharedPointer>
#include <QWidget>

#include "common.h"

namespace gams {
namespace studio{
namespace mii {

namespace Ui {
class ModelInspector;
}

class AbstractViewFrame;
class AbstractModelInstance;
class AbstractViewConfiguration;
class SectionTreeModel;
class SearchResultModel;

class ModelInspector final : public QWidget
{
    Q_OBJECT

public:
    explicit ModelInspector(QWidget *parent = nullptr);
    ~ModelInspector() override;

    QString scratchDir() const;
    void setScratchDir(const QString &scratchDir);

    QString workspace() const;
    void setWorkspace(const QString &workspace);

    QString systemDirectory() const;
    void setSystemDirectory(const QString &systemDir);

    bool showOutput() const;
    void setShowOutput(bool showOutpu);
    
    void setShowAbsoluteValuesGlobal(bool absoluteValues);

    SearchResult& searchHeaders(const QString &term, bool isRegEx);
    SearchResult& searchResult();

    ViewActionStates viewActionStates() const;

    void loadModelInstance(bool loadModel = true);
    void reloadModelInstance();

    QSharedPointer<AbstractViewConfiguration> viewConfig();

    void cancelRun();

    void zoomIn();
    void zoomOut();
    void resetZoom();

    void updateFilters();

signals:
    void filtersChanged();

    void openFilterDialog();

    void viewChanged(int);

    void newLogMessage(const QString&);

    void dataLoaded();

public slots:
    void saveModelView();

    void createNewSymbolView();

    void removeModelView();

    void setCurrentView();

    void setSearchSelection(const gams::studio::mii::SearchResult::SearchEntry &result);

private slots:
    void selectScalingView();

private:
    void setupConnections();

    void setupModelInstanceView(bool loadModel);

    void clearDefaultViewData();

    void clearCustomViews();

    void setCurrentViewIndex(ViewHelper::ViewType viewType, ViewHelper::ViewDataType viewDataType);

    AbstractViewFrame* currentView() const;

    int currentViewIndex(AbstractViewFrame* view) const;

private:
    Ui::ModelInspector* ui;
    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;
    SearchResult mDefaultSearchResult;

    SectionTreeModel* mSectionModel = nullptr;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QFuture<void> mFutureData;
};

}
}
}

#endif // MODELINSPECTOR_H
