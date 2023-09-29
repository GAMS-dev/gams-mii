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
#include "aggregation.h"

namespace gams {
namespace studio{
namespace mii {

namespace Ui {
class ModelInspector;
}

class AbstractViewFrame;
class AbstractModelInstance;
class SectionTreeModel;
class SearchResultModel;

class ModelInspector : public QWidget
{// TODO !!! use ViewConfig in MI interface?
    Q_OBJECT

public:
    explicit ModelInspector(QWidget *parent = nullptr);
    ~ModelInspector();

    QString scratchDir() const;
    void setScratchDir(const QString &scratchDir);

    QString workspace() const;
    void setWorkspace(const QString &workspace);

    QString systemDirectory() const;
    void setSystemDirectory(const QString &systemDir);

    bool showOutput() const;
    void setShowOutput(bool showOutpu);

    void setShowAbsoluteValues(bool absoluteValues);

    SearchResult& searchHeaders(const QString &term, bool isRegEx);
    SearchResult& searchResult();

    ViewActionStates viewActionStates() const;

    void loadModelInstance(bool loadModel = true);
    void reloadModelInstance();
    QSharedPointer<AbstractModelInstance> modelInstance() const;

    IdentifierFilter identifierFilter() const;
    IdentifierFilter defaultIdentifierFilter() const;
    void setIdentifierFilter(const IdentifierFilter &filter);

    ValueFilter valueFilter() const;
    ValueFilter defaultValueFilter() const;
    void setValueFilter(const ValueFilter &filter);

    LabelFilter labelFilter() const;
    LabelFilter defaultLabelFilter() const;
    void setLabelFilter(const LabelFilter &filter);

    Aggregation aggregation() const;
    Aggregation defaultAggregation() const;
    void setAggregation(const Aggregation &aggregation);

    ViewDataType viewType() const;

    void resetColumnRowFilter();

    void cancelRun();

    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void filtersChanged();

    void viewChanged(int);

    void newLogMessage(const QString&);

    void dataLoaded();

public slots:
    void saveModelView();
    void createNewSymbolView();

    void removeModelView();

    void setCurrentView(int index);

    void setCurrentViewIndex(gams::studio::mii::ViewType type);

    void setSearchSelection(const gams::studio::mii::SearchResult::SearchEntry &result);

private slots:
    void updateView();

private:
    void setupConnections();

    void setupModelInstanceView(bool loadModel);

    void clearCustomViews();

    AbstractViewFrame* currentView() const;

private:
    Ui::ModelInspector* ui;
    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;
    SearchResult mDefaultSearchResult;

    SectionTreeModel* mSectionModel = nullptr;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QMap<int, AbstractViewFrame*> mCustomViews;
    QFuture<void> mFutureData;
};

}
}
}

#endif // MODELINSPECTOR_H
