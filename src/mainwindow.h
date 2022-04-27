/*
 * This file is part of the GAMS Studio project.
 *
 * Copyright (c) 2017-2018 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2017-2018 GAMS Development Corp. <support@gams.com>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QProcess>
#include <QMainWindow>
#include <QSharedPointer>

class QLabel;

namespace Ui {
class MainWindow;
}

class GAMSLibProcess;
class GAMSProcess;

namespace gams {
namespace studio {
namespace modelinspector {
class AggregationDialog;
class GlobalFilterDialog;
}
}
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void appendLogMessage(const QString &message);

    // File
    void on_actionOpen_triggered();
    void on_actionRun_triggered();
    void on_action_Quit_triggered();

    // Edit
    void on_action_Search_triggered();
    void searchHeaders();

    // View
    void on_actionGlobal_Filters_triggered();
    void on_actionAggregation_triggered();
    void on_actionShow_search_result_triggered();
    void on_actionShow_Output_triggered();

    // Help
    void on_actionAbout_Model_Inspector_triggered();
    void on_actionAbout_Qt_triggered();

    // Other
    void loadModelInstance(int exitCode, QProcess::ExitStatus exitStatus);
    void aggregationUpdate();
    void globalFilterUpdate();
    void searchResultSelectionChanged(const QModelIndex &index);
    void updateModelInstance();

private:
    QString aboutModelInspector() const;

    void loadGAMSModel(const QString &path);

    void setGlobalFiltersData();
    void setAggregationData();

    void showDialog(QDialog *dialog);

    QString workspace() const;

private:
    Ui::MainWindow *ui;
    GAMSLibProcess *mLibProcess;
    QSharedPointer<GAMSProcess> mProcess;
    gams::studio::modelinspector::AggregationDialog *mAggregationDialog;
    gams::studio::modelinspector::GlobalFilterDialog *mGlobalFilterDialog;
    QLabel *mAggregationStatusLabel;
};

#endif // MAINWINDOW_H
