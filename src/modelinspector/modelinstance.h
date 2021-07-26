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

#include <functional>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

class QStandardItem;
class QStandardItemModel;

namespace gams {
namespace studio {
namespace modelinspector {

struct ValueFilterSettings;

// TODO Which default values if something goes wrong (e.g. excel '##ERROR##', -1, ...)? -> something like '##ERROR##'

struct SymbolInfo
{
    int Index = -1;
    int Offset = -1;
    int Entries = -1;
    QString Name;
    int Dimension = -1;
    int Type = -1;

    bool isScalar() const {
        return Entries == 1;
    }

    int lastOffset() const {
        return Offset+Entries;
    }
};

struct MaxMin {
    bool Valid = false;
    double Max;
    double Min;
};

class ModelInstance
{
public:
    ModelInstance(const QString &workingDir = ".", const QString &scratchDir = QString());

    ~ModelInstance();

    QString modelName() const;

    int coefficents() const;

    int positiveCoefficents() const;

    int negativeCoefficents() const;

    int nonLinearCoefficents() const;

    /**
     * @brief Total number of equations.
     */
    int equations() const;

    /**
     * @brief Number of equations defined by <c>type</c>.
     * @param Equation type.
     */
    int equations(int type) const;

    int equationBlocks();

    bool isEquation(int type);

    bool isEquation(const QString &name);

    /**
     * @brief Total number of variables.
     */
    int variables() const;

    /**
     * @brief Number of variables defined by <c>type</c>.
     * @param Equation type.
     */
    int variables(int type) const;

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

    SymbolInfo symbol(int index) const;

    SymbolInfo symbol(int index, int type);

    const QVector<SymbolInfo>& symbols(int type) const;

    void loadScratchData();
    void loadSymbols();
    void loadMinMaxValues();

    QPair<double, double> matrixRange() const;

    QPair<double, double> objectiveRange() const;

    QPair<double, double> boundsRange() const;

    QPair<double, double> rhsRange() const;

    int rowCount();

    int columnCount();

    void horizontalHeaderData(QStandardItemModel &model);
    void verticalHeaderData(QStandardItemModel &model);

    QStandardItem* horizontalItem(int logicalIndex);
    QStandardItem* verticalItem(int logicalIndex);

    QList<QStandardItem*> horizontalItems();
    QList<QStandardItem*> verticalItems();

    QList<QStandardItem*> horizontalUelItems();
    QList<QStandardItem*> verticalUelItems();

    QList<QStandardItem*> horizontalSymbols() const;
    QList<QStandardItem*> verticalSymbols() const;

    QList<QStandardItem*> setBranchState(QStandardItem *startItem, Qt::CheckState state);

    void setUelStates(Qt::Orientation orientation, const QMap<QString, bool> &uelStates);
    QMap<QString, bool> uelStates(Qt::Orientation orientation);

    // TOOD merge the two functions?
    int horizontalIndex(QStandardItem *item);
    int verticalIndex(QStandardItem *item);
    int itemToIndex(Qt::Orientation orientation, QStandardItem *item);
    void setHeaderRootItemEnabled(QStandardItem *item, bool enabled);

    int predefinedHeaderLength() const;

    QVariant data(int row, int column);

    ValueFilterSettings valueFilterSettings() const;
    void setValueFilterSettings(const ValueFilterSettings &settings);

private:
    void initialize();

    double jaccobianValue(int row, int column);
    QVariant variableAttribute(int row, int column);
    QVariant equationAttribute(int row, int column);

    QPair<double, double> equationBounds(int row);

    void appendHeaderColumns(QStandardItem *parent, const SymbolInfo &symbolInfo,
                             Qt::Orientation orientation);

    void appendHeaderItems(QStandardItem *parent, QStringList &uels);

    QVariant specialValue(double value);
    QVariant specialMarginalValue(double value);
    QVariant specialMarginalEquValueBasis(double value, int rIndex);
    QVariant specialMarginalVarValueBasis(double value, int cIndex);

    void setItemToIndexMapping(QStandardItem *item, QMap<QStandardItem*, int> &mapping);

    static int errorCallback(int count, const char *message);

private:
    class Cache;
    Cache *mCache;

    QString mScratchDir;
    QString mWorkspace;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
    dctHandle_t mDCT = nullptr;
    std::function<QVariant(double, int)> specialMarginalEquValuePtr;
    std::function<QVariant(double, int)> specialMarginalVarValuePtr;

    QStringList mLogMessages;

    const static QStringList PredefinedHeader;
    const static int PredefinedHeaderLength;
};

}
}
}

#endif // MODELINSTANCE_H
