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
class FilterDialog;
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
    void on_actionOpen_Project_triggered();
    void on_actionRun_triggered();
    void on_action_Quit_triggered();

    // Edit
    void on_action_Search_triggered();
    void searchHeaders();
    void editMenuAboutToShow();

    // View
    void on_actionFilters_triggered();
    void on_actionAggregation_triggered();
    void on_actionReset_default_views_triggered();
    void on_actionShow_search_result_triggered();
    void showAbsoluteValues();
    void on_actionShow_Output_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_action_Actual_Size_triggered();

    // Help
    void on_actionAbout_Model_Inspector_triggered();
    void on_actionAbout_Qt_triggered();

    // Other
    void loadModelInstance(int exitCode, QProcess::ExitStatus exitStatus);
    void aggregationUpdate();
    void globalFilterUpdate();
    void setGlobalFiltersData();
    void searchResultSelectionChanged(const QModelIndex &index);
    void updateModelInstance();
    void viewChanged(int viewType);

private:
    void setupConnections();

    void createProjectDirectory();

    QString aboutModelInspector() const;

    void loadGAMSModel(const QString &path);

    void setAggregationData();

    void showDialog(QDialog *dialog);

    QString workspace() const;

    QString projectDirectory() const;

private:
    Ui::MainWindow *ui;
    GAMSLibProcess *mLibProcess;
    QSharedPointer<GAMSProcess> mProcess;
    gams::studio::modelinspector::AggregationDialog *mAggregationDialog;
    gams::studio::modelinspector::FilterDialog *mFilterDialog;
    QLabel *mAggregationStatusLabel;
};

#endif // MAINWINDOW_H
