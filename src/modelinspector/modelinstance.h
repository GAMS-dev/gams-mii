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
#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "gevmcc.h"
#include "gmomcc.h"
#include "dctmcc.h"

#include "common.h"
#include "symbolinfo.h"
#include "aggregation.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QVariant>

class QAbstractItemModel;

namespace gams {
namespace studio {
namespace modelinspector {

class DataHandler;

struct ValueFilterSettings;

// TODO (AF) Which default values if something goes wrong (e.g. excel '##ERROR##', -1, ...)? -> something like '##ERROR##'

struct MaxMin
{
    bool Valid = false;
    double Max;
    double Min;
};

class ModelInstance
{
public:
    ModelInstance(const QString &workingDir = ".",
                  const QString &systemDir = QString(),
                  const QString &scratchDir = QString());

    ~ModelInstance();

    void initialize();

    bool isInitialized() const
    {
        return mInitialized;
    }

    bool useOutput() const
    {
        return mUseOutput;
    }

    void setUseOutput(bool useOutput)
    {
        mUseOutput = useOutput;
    }

    QString workspace() const
    {
        return mWorkspace;
    }

    void setWorkspace(const QString &workspace)
    {
        mWorkspace = QDir(workspace).absolutePath();
    }

    QString systemDirectory() const
    {
        return mSystemDir;
    }

    QString scratchDirectory() const
    {
        return mScratchDir;
    }

    void setScratchDirectory(const QString &scratchDir)
    {
        mScratchDir = scratchDir;
    }

    void setSystemDirectory(const QString &systemDir)
    {
        mSystemDir = systemDir;
    }

    QString modelName() const;

    int coefficents() const;

    int positiveCoefficents() const;

    int negativeCoefficents() const;

    int nonLinearCoefficents() const;

    SymbolInfo equation(int sectionIndex) const;

    const QVector<SymbolInfo>& equations() const;

    /**
     * @brief Total number of equations.
     */
    int equationCount() const;

    /**
     * @brief Number of equations defined by <c>type</c>.
     * @param Equation type.
     */
    int equationCount(int type) const;

    int equationBlocks();

    bool isEquation(int type);

    bool isEquation(const QString &name);

    QString longestEqnText() const;

    QString longestVarText() const;

    SymbolInfo variable(int sectionIndex) const;

    const QVector<SymbolInfo>& variables() const;

    /**
     * @brief Total number of variables.
     */
    int variableCount() const;

    int maxEquationDimension() const;

    int maxVariableDimension() const;

    /**
     * @brief Number of variables defined by <c>type</c>.
     * @param Equation type.
     */
    int variableCount(int type) const;

    int variableBlocks();

    bool isVariable(int symType) const {
        return symType == dctvarSymType ? true : false;
    }

    bool isVariable(const QString &name);

    int symbolCount() const {
        return dctNLSyms(mDCT);
    }

    QString equationType(int offset) const;

    QVector<QVariant> scalarEquationData(int offset) const;
    QMap<int,QVariant> equationData(int currentRow) const;

    QVector<MaxMin> equationVariableScaling(const SymbolInfo &symbol);
    QVector<MaxMin> totalScaling();
    MaxMin equationScaling(const SymbolInfo &symbol);

    QPair<double,double> maxminRhs(int offset, int entries) const;
    QPair<double,double> totalRhs();
    QString aggregatedRhs(const SymbolInfo &symbol) const;

    int rowIndex(int offset) const;

    QString logMessages() {
        auto messages = mLogMessages.join("\n");
        mLogMessages.clear();
        return messages;
    }

    QStringList symbolNames() const;

    int symbolIndex(const QString &label) const;

    /**
    * @brief Symbol offset to get records.
    * @return dctSymOffset, offset < 0 is failiure
    */
    int symbolOffset(const QString &label) const;

    const SymbolInfo& symbol(int index, int type) const;

    const QVector<SymbolInfo>& symbols(int type) const;

    void loadScratchData(bool useOutput);
    void loadTableData(LabelFilter &labelFilter);

    QPair<double, double> matrixRange() const;

    QPair<double, double> objectiveRange() const;

    QPair<double, double> boundsRange() const;

    QPair<double, double> rhsRange() const;

    int rowCount(PredefinedViewEnum viewType) const;
    int jaccRowCount() const;

    int columnCount(PredefinedViewEnum viewType) const;
    int jaccColumnCount() const;

    QVariant data(int row, int column, int view) const;

    void aggregate(const Aggregation &aggregation);

    int headerData(int logicalIndex, Qt::Orientation orientation, int view) const;

    QString headerData(int sectionIndex, int dimension, Qt::Orientation orientation) const;

    void searchHeaderData(int logicalIndex, int sectionIndex, const QString &term,
                          bool isRegEx, DataSource symbolType, Qt::Orientation orientation,
                          QList<SearchResult> &result);

    DataRow jaccobianRow(int row);

    QVariant horizontalAttribute(const QString &header, int column);
    QVariant verticalAttribute(const QString &header, int row);

    double modelMinimum() const
    {
        return mModelMinimum;
    }

    double modelMaximum() const
    {
        return mModelMaximum;
    }

private:
    SymbolInfo loadSymbol(int index, int sectionIndex) const;
    void loadDimensions(SymbolInfo &symbol) const;
    void loadInitialLabelFilter(Qt::Orientation orientation, LabelFilter &labelFilter);

    void loadLabelTree(SymbolInfo &symbol) const;
    void appendSubItems(LabelTreeItem *parent, QStringList &labels) const;

    QPair<double, double> equationBounds(int row);

    QVariant specialValue(double value);
    QVariant specialValueMinMax(double value);
    QVariant specialMarginalValue(double value);
    QVariant specialMarginalEquValueBasis(double value, int rIndex);
    QVariant specialMarginalVarValueBasis(double value, int cIndex);

    static int errorCallback(int count, const char *message);

private:
    class SymbolCache;
    SymbolCache *mSymbolCache;

    bool mInitialized = false;
    bool mUseOutput = false;

    double mModelMinimum = 0.0;
    double mModelMaximum = 0.0;

    DataHandler *mDataHandler;

    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
    dctHandle_t mDCT = nullptr;
    std::function<QVariant(double, int)> specialMarginalEquValuePtr;
    std::function<QVariant(double, int)> specialMarginalVarValuePtr;

    QStringList mLogMessages;
};

}
}
}

#endif // MODELINSTANCE_H
