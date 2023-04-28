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

#include "abstractmodelinstance.h"

#include "gevmcc.h"
#include "gmomcc.h"
#include "dctmcc.h"

#include <QVariant>

namespace gams {
namespace studio {
namespace modelinspector {

class DataHandler;
class DataMatrix;

class ModelInstance : public AbstractModelInstance
{
public:
    ModelInstance(const QString &workspace = ".",
                  const QString &systemDir = QString(),
                  const QString &scratchDir = QString());

    ~ModelInstance();

    QString modelName() const override;

    int equationCount() const override;

    int equationCount(EquationType type) const override;

    char equationType(int row) const override;

    int equationRowCount() const override;

    Symbol* equation(int sectionIndex) const override;

    const QVector<Symbol*>& equations() const override;

    int variableCount() const override;

    int variableCount(VariableType type) const override;

    char variableType(int column) const override;

    int variableRowCount() const override;

    Symbol* variable(int sectionIndex) const override;

    const QVector<Symbol*>& variables() const override;

    int coefficents() const override;

    int positiveCoefficents() const override;

    int negativeCoefficents() const override;

    int nonLinearCoefficents() const override;

    Range matrixRange() const override;

    Range objectiveRange() const override;

    Range boundsRange() const override;

    void variableLowerBounds(double *bounds) override;

    void variableUpperBounds(double *bounds) override;

    Range rhsRange() const override;

    double rhs(int row) const override;

    QString longestEquationText() const override;

    QString longestVariableText() const override;

    int maximumEquationDimension() const override;

    int maximumVariableDimension() const override;

    const QVector<Symbol*>& symbols(Symbol::Type type) const override;

    void loadData() override;

    int rowCount(int view) const override;

    int rowEntries(int row, int view) const override;

    int columnCount(int view) const override;

    int columnEntries(int column, int view) const override;

    QSharedPointer<AbstractViewConfiguration> clone(int view, int newView) override;

    void loadData(QSharedPointer<AbstractViewConfiguration> viewConfig) override;

    QVariant data(int row, int column, int view) const override;

    QVariant headerData(int logicalIndex,
                        Qt::Orientation orientation,
                        int view, int role) const override;

    QVariant plainHeaderData(Qt::Orientation orientation,
                             int view, int logicalIndex, int dimension) const override;

    void jaccobianData(DataMatrix& dataMatrix) override;

private:
    void initialize();

    int symbolCount() const;

    void loadScratchData();

    void loadSymbols();
    Symbol* loadSymbol(int index);
    void loadEquationDimensions(Symbol *symbol);
    void loadVariableDimensions(Symbol *symbol);
    void loadLabels();

    QPair<double, double> equationBounds(int row);

    QVariant specialValue(double value);
    QVariant specialValueMinMax(double value);
    QVariant specialMarginalValue(double value);
    QVariant specialMarginalEquValueBasis(double value, int rIndex);
    QVariant specialMarginalVarValueBasis(double value, int cIndex);

    static int errorCallback(int count, const char *message);

private:
    DataHandler *mDataHandler;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
    dctHandle_t mDCT = nullptr;
    std::function<QVariant(double, int)> specialMarginalEquValuePtr;
    std::function<QVariant(double, int)> specialMarginalVarValuePtr;

    int mMaxEquationDimension = 0;
    int mMaxVariableDimension = 0;

    QList<Symbol*> hSectionIndexToSymbol;
    QList<Symbol*> vSectionIndexToSymbol;

    QVector<Symbol*> mEquations;
    QVector<Symbol*> mVariables;

    QString mLongestLabel;
    QString mLongestEqnText;
    QString mLongestVarText;
};

}
}
}

#endif // MODELINSTANCE_H
