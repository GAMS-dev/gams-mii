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
#include "modelstatistic.h"
#include "sectiontreemodel.h"

#include "gclgms.h"

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelInspector)
    , mSectionModel(new SectionTreeModel)
    , mModelInstance(new ModelInstance("."))
{
    ui->setupUi(this);
    mSectionModel->loadModelData({"Statistic", "Statistic 2", "Blockpic?"});
    ui->sectionTreeView->setModel(mSectionModel);

    connect(ui->sectionTreeView, &SectionTreeView::currentItemChanged,
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

ModelInspector::~ModelInspector()
{
    delete ui;
}

QString ModelInspector::scratchDir() const
{
    return mModelInstance->scratchDir();
}

void ModelInspector::setScratchDir(const QString &scratchDir)
{
    mModelInstance->setScratchDir(scratchDir);
}

QString ModelInspector::workspace() const
{
    return mWorkspace;
}

void ModelInspector::setWorkspace(const QString &workingDir)
{
    mWorkspace = workingDir;
}

void ModelInspector::releasePreviousModel()
{
    mModelInstance = std::make_unique<ModelInstance>(ModelInstance(mWorkspace));
}

void ModelInspector::showModelStatisics()
{
    mModelInstance->loadScratchData();
    ui->statisticWidget->showStatistic(mModelInstance.get());
    showStatisitics2();
    emit newLogMessage(mModelInstance->logMessages());
}

void ModelInspector::showStatisitics2()
{
    ui->statistic2Widget->setColumnCount(3);
    ui->statistic2Widget->setRowCount(33);

    ModelStatistic statistics(mModelInstance.get());
    auto matrixRange = statistics.matrixRange();
    auto objectiveRange = statistics.objectiveRange();
    auto boundsRange = statistics.boundsRange();
    auto rhsRange = statistics.rhsRange();

    auto font = QFont();
    font.setBold(true);

    //ui->statistic2Widget->setItem(0, 0, new QTableWidgetItem(""));
    auto item1 = new QTableWidgetItem("Equation Counts");
    item1->setFont(font);
    ui->statistic2Widget->setItem(0, 0, item1);
    ui->statistic2Widget->setItem(1, 0, new QTableWidgetItem("Blocks"));
    ui->statistic2Widget->setItem(2, 0, new QTableWidgetItem("Individual"));

    auto item = new QTableWidgetItem("=E=");
    item->setToolTip("Equality: right-hand side must equal left-hand side");
    ui->statistic2Widget->setItem(3, 0, item);

    item = new QTableWidgetItem("=G=");
    item->setToolTip("Greater than: left-hand side must be greater than or equal to right-hand side");
    ui->statistic2Widget->setItem(4, 0, item);

    item = new QTableWidgetItem("=L=");
    item->setToolTip("Less than: left-hand side must be less than or equal to right-hand side");
    ui->statistic2Widget->setItem(5, 0, item);

    item = new QTableWidgetItem("=N=");
    item->setToolTip("No relationship implied between left-hand side and right-hand side. This equation type is ideally suited for use in MCP models and in variational inequalities.");
    ui->statistic2Widget->setItem(6, 0, item);

    item = new QTableWidgetItem("=X=");
    item->setToolTip("Equation is defined by external programs");
    ui->statistic2Widget->setItem(7, 0, item);

    item = new QTableWidgetItem("=C=");
    item->setToolTip("Conic constraint");
    ui->statistic2Widget->setItem(8, 0, item);

    item = new QTableWidgetItem("=B=");
    item->setToolTip("Boolean equation");
    ui->statistic2Widget->setItem(9, 0, item);

    ui->statistic2Widget->setItem(1, 1, new QTableWidgetItem(QString::number(statistics.equationBlocks())));
    ui->statistic2Widget->setItem(2, 1, new QTableWidgetItem(QString::number(statistics.equations())));
    ui->statistic2Widget->setItem(3, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_E))));
    ui->statistic2Widget->setItem(4, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_G))));
    ui->statistic2Widget->setItem(5, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_L))));
    ui->statistic2Widget->setItem(6, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_N))));
    ui->statistic2Widget->setItem(7, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_X))));
    ui->statistic2Widget->setItem(8, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_C))));
    ui->statistic2Widget->setItem(9, 1, new QTableWidgetItem(QString::number(statistics.numberEquations(gmoequ_B))));

    auto item2 = new QTableWidgetItem("Variable Counts");
    item2->setFont(font);
    ui->statistic2Widget->setItem(11, 0, item2);
    ui->statistic2Widget->setItem(12, 0, new QTableWidgetItem("Blocks"));
    ui->statistic2Widget->setItem(13, 0, new QTableWidgetItem("Individuals"));
    ui->statistic2Widget->setItem(14, 0, new QTableWidgetItem("Continous"));
    ui->statistic2Widget->setItem(15, 0, new QTableWidgetItem("Binary"));
    ui->statistic2Widget->setItem(16, 0, new QTableWidgetItem("Integer"));

    item = new QTableWidgetItem("SOS1");
    item->setToolTip("Special Order Sets of Type 1");
    ui->statistic2Widget->setItem(17, 0, item);

    item = new QTableWidgetItem("SOS2");
    item->setToolTip("Special Order Sets of Type 2");
    ui->statistic2Widget->setItem(18, 0, item);

    item = new QTableWidgetItem("SCont");
    item->setToolTip("Semi-Continous Variables");
    ui->statistic2Widget->setItem(19, 0, item);

    item = new QTableWidgetItem("SInt");
    item->setToolTip("Semi-Integer Variables");
    ui->statistic2Widget->setItem(20, 0, item);

    ui->statistic2Widget->setItem(12, 1, new QTableWidgetItem(QString::number(statistics.variableBlocks())));
    ui->statistic2Widget->setItem(13, 1, new QTableWidgetItem(QString::number(statistics.variables())));
    ui->statistic2Widget->setItem(14, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_X))));
    ui->statistic2Widget->setItem(15, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_B))));
    ui->statistic2Widget->setItem(16, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_I))));
    ui->statistic2Widget->setItem(17, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_S1))));
    ui->statistic2Widget->setItem(18, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_S2))));
    ui->statistic2Widget->setItem(19, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_SC))));
    ui->statistic2Widget->setItem(20, 1, new QTableWidgetItem(QString::number(statistics.numberVariables(gmovar_SI))));

    auto item3 = new QTableWidgetItem("Coefficient Counts");
    item3->setFont(font);
    ui->statistic2Widget->setItem(22, 0, item3);
    ui->statistic2Widget->setItem(23, 0, new QTableWidgetItem("Total"));
    ui->statistic2Widget->setItem(24, 0, new QTableWidgetItem("Positive"));
    ui->statistic2Widget->setItem(25, 0, new QTableWidgetItem("Negative"));
    ui->statistic2Widget->setItem(26, 0, new QTableWidgetItem("Non-Linear"));
    ui->statistic2Widget->setItem(23, 1, new QTableWidgetItem(QString::number(statistics.coefficents())));
    ui->statistic2Widget->setItem(24, 1, new QTableWidgetItem(QString::number(statistics.positiveCoefficents())));
    ui->statistic2Widget->setItem(25, 1, new QTableWidgetItem(QString::number(statistics.negativeCoefficents())));
    ui->statistic2Widget->setItem(26, 1, new QTableWidgetItem(QString::number(statistics.nonLinearCoefficents())));

    auto item4 = new QTableWidgetItem("Coefficient Statistics");
    item4->setFont(font);
    ui->statistic2Widget->setItem(28, 0, item4);
    ui->statistic2Widget->setItem(29, 0, new QTableWidgetItem("Matrix Range"));
    ui->statistic2Widget->setItem(30, 0, new QTableWidgetItem("Objective Range"));
    ui->statistic2Widget->setItem(31, 0, new QTableWidgetItem("Bounds Range"));
    ui->statistic2Widget->setItem(32, 0, new QTableWidgetItem("RHS Range"));

    ui->statistic2Widget->setItem(29, 1, new QTableWidgetItem(QString::number(matrixRange.first)));
    ui->statistic2Widget->setItem(30, 1, new QTableWidgetItem(QString::number(objectiveRange.first)));
    ui->statistic2Widget->setItem(31, 1, new QTableWidgetItem(QString::number(boundsRange.first)));
    ui->statistic2Widget->setItem(32, 1, new QTableWidgetItem(QString::number(rhsRange.first)));

    ui->statistic2Widget->setItem(29, 2, new QTableWidgetItem(QString::number(matrixRange.second)));
    ui->statistic2Widget->setItem(30, 2, new QTableWidgetItem(QString::number(objectiveRange.second)));
    ui->statistic2Widget->setItem(31, 2, new QTableWidgetItem(QString::number(boundsRange.second)));
    ui->statistic2Widget->setItem(32, 2, new QTableWidgetItem(QString::number(rhsRange.second)));

    ui->statistic2Widget->resizeColumnsToContents();

    QMap<EquationType, double> equationData {
        { EquationType::E, statistics.numberEquations(gmoequ_E) },
        { EquationType::G, statistics.numberEquations(gmoequ_G) },
        { EquationType::L, statistics.numberEquations(gmoequ_L) },
        { EquationType::N, statistics.numberEquations(gmoequ_N) },
        { EquationType::X, statistics.numberEquations(gmoequ_X) },
        { EquationType::C, statistics.numberEquations(gmoequ_C) },
        { EquationType::B, statistics.numberEquations(gmoequ_B) }
    };
    ui->equationView->setData(equationData);

    QMap<VariableType, double> variableData {
        { VariableType::Continous, statistics.numberVariables(gmovar_X) },
        { VariableType::Binary, statistics.numberVariables(gmovar_B) },
        { VariableType::Integer, statistics.numberVariables(gmovar_I) },
        { VariableType::SOS1, statistics.numberVariables(gmovar_S1) },
        { VariableType::SOS2, statistics.numberVariables(gmovar_S2) },
        { VariableType::SCont, statistics.numberVariables(gmovar_SC) },
        { VariableType::SInt, statistics.numberVariables(gmovar_SI) }
    };
    ui->variableView->setData(variableData);

    QMap<CoefficientType, double> coefficientData {
        { CoefficientType::Positive, statistics.positiveCoefficents() },
        { CoefficientType::Negative, statistics.negativeCoefficents() },
        { CoefficientType::NonLinear, statistics.nonLinearCoefficents() }
    };
    ui->coeffView->setData(coefficientData);
}

}
}
}
