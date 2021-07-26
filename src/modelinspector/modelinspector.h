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

#include <QProcess>
#include <QSharedPointer>
#include <QWidget>

#include "searchresult.h"

class QStandardItem;

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class ModelInspector;
}

class FilterTreeItem;
class ModelInstance;
class ModelInstanceTableModel;
class SectionTreeModel;
class SearchResultModel;
class ValueFormatProxyModel;
struct ValueFilterSettings;

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

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx);

    QSharedPointer<ModelInstance> modelInstance() const;

signals:
    void filtersUpdated();

    // TODO (AF) use message types when integrated into studio
    void newLogMessage(const QString&);
    void newModelInstance();

public slots:
    void loadModelInstance(int exitCode, QProcess::ExitStatus status);
    void releasePreviousModel();

    void processGlobalFilterUpdate();

    void setCurrentView(int index);

    void setSearchSelection(const SearchResult &result);

private slots:
    void applyHeaderLabelFilter(FilterTreeItem *root, Qt::Orientation orientation);

private:
    void applyHeaderSymbolFilter();

private:
    Ui::ModelInspector* ui;
    QString mScratchDir;
    QString mWorkspace;
    SectionTreeModel *mSectionModel;
    QSharedPointer<ModelInstance> mModelInstance;
    QSharedPointer<ModelInstanceTableModel> mModelInstanceModel;
    ValueFormatProxyModel *mValueFormatModel;
};

}
}
}

#endif // MODELINSPECTOR_H
