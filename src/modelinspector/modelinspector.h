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

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class ModelInspector;
}

class ModelInstance;
class ModelInstanceTableModel;
class SectionTreeModel;

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

signals:
    // TODO (AF) split/use enum for error/info/...?
    void newLogMessage(const QString&);

public slots:
    void loadModelInstance(int exitCode, QProcess::ExitStatus status);
    void releasePreviousModel();

    void setCurrentView(int index);

private:
    Ui::ModelInspector* ui;
    QString mScratchDir;
    QString mWorkspace;
    SectionTreeModel *mSectionModel;
    QSharedPointer<ModelInstance> mModelInstance;
    QSharedPointer<ModelInstanceTableModel> mModelInstanceModel;
};

}
}
}

#endif // MODELINSPECTOR_H
