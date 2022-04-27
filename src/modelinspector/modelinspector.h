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
#ifndef MODELINSPECTOR_H
#define MODELINSPECTOR_H

#include <QSharedPointer>
#include <QWidget>

#include "common.h"
#include "aggregation.h"

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class ModelInspector;
}

class ModelInstance;
class ModelInstanceTableModel;
class SectionTreeModel;
class SearchResultModel;
class ColumnRowFilterModel;
class IdentifierFilterModel;
class IdentifierLabelFilterModel;
class ValueFormatProxyModel;
class LabelFilterModel;
class AggregationProxyModel;
class HierarchicalHeaderView;

class ModelInspector : public QWidget
{
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

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx);

    void loadModelInstance();
    void reloadModelInstance();
    QSharedPointer<ModelInstance> modelInstance() const;

    IdentifierFilter identifierFilter() const;
    IdentifierFilter defaultIdentifierFilter();
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

    void resetColumnRowFilter();

signals:
    void newLogMessage(const QString&); // TODO (AF) use message types when integrated into studio

    void filtersChanged();

    void viewChanged(PredefinedViewEnum);

public slots:
    void setCurrentView(int index);

    void setSearchSelection(const gams::studio::modelinspector::SearchResult &result);

private slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation);

private:
    void setupModelInstanceView();

    void searchHeader(const QString &term, bool isRegEx,
                      Qt::Orientation orientation,
                      QList<SearchResult> &result);

    Aggregation appliedAggregation(const Aggregation &aggregations) const;
    Aggregation initialAggregation() const;

    LabelFilter initialLabelFilter() const;

    IdentifierStates initialSymbolFilter(QAbstractItemModel *model,
                                         Qt::Orientation orientation) const;

    ValueFilter initalValueFilter() const;

private:
    Ui::ModelInspector* ui;
    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;
    bool mShowOutput = false;
    SectionTreeModel* mSectionModel = nullptr;
    QSharedPointer<ModelInstance> mModelInstance;
    QSharedPointer<ModelInstanceTableModel> mModelInstanceModel;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    ColumnRowFilterModel* mColumnRowFilterModel = nullptr;
    IdentifierFilterModel* mIdentifierFilterModel = nullptr;
    IdentifierLabelFilterModel* mIdentifierLabelFilterModel = nullptr;
    LabelFilterModel* mLabelFilterModel = nullptr;
    AggregationProxyModel* mAggregationModel = nullptr;
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;

    LabelFilter mInitialLabelFilter;
    ValueFilter mInitialValueFilter;
};

}
}
}

#endif // MODELINSPECTOR_H
