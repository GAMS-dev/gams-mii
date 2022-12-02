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

struct ValueFilterSettings;

struct MaxMin
{
    bool Valid = false;
    double Max;
    double Min;
};

class ModelInstance : public AbstractModelInstance
{
public:
    ModelInstance(const QString &workspace = ".",
                  const QString &systemDir = QString(),
                  const QString &scratchDir = QString());

    ~ModelInstance();

    void initialize() override;

    QString modelName() const override;

    int equationCount() const override;

    int equationCount(EquationType type) const override;

    int equationRowCount() const override;

    Symbol equation(int sectionIndex) const override;

    const QVector<Symbol>& equations() const override;

    int variableCount() const override;

    int variableCount(VariableType type) const override;

    int variableRowCount() const override;

    Symbol variable(int sectionIndex) const override;

    const QVector<Symbol>& variables() const override;

    int coefficents() const override;

    int positiveCoefficents() const override;

    int negativeCoefficents() const override;

    int nonLinearCoefficents() const override;

    Range matrixRange() const override;

    Range objectiveRange() const override;

    Range boundsRange() const override;

    Range rhsRange() const override;

    QString longestEquationText() const override;

    QString longestVariableText() const override;

    int maximumEquationDimension() const override;

    int maximumVariableDimension() const override;

    const QVector<Symbol>& symbols(Symbol::Type type) const override;

    void loadData(LabelFilter &labelFilter) override;

    int rowCount(PredefinedViewEnum viewType) const override;

    int columnCount(PredefinedViewEnum viewType) const override;

    QVariant data(int row, int column, int view) const override;

    QString headerData(int sectionIndex,
                       int dimension,
                       Qt::Orientation orientation) const override;

    void aggregate(const Aggregation &aggregation) override;

    int headerData(int logicalIndex,
                   Qt::Orientation orientation,
                   int view) const override;


    void searchHeaderData(int logicalIndex,
                          int sectionIndex,
                          const QString &term,
                          bool isRegEx,
                          DataSource symbolType,
                          Qt::Orientation orientation,
                          QList<SearchResult> &result) override;


    // TODO review,move,scope: used by DataHandler only
    DataRow jaccobianRow(int row);
    QVariant horizontalAttribute(const QString &header, int column);
    QVariant verticalAttribute(const QString &header, int row);

private:
    int symbolCount() const;

    void loadScratchData();
    void loadTableData(LabelFilter &labelFilter);

    Symbol loadSymbol(int index, int sectionIndex);
    void loadDimensions(Symbol &symbol);
    void loadInitialLabelFilter(Qt::Orientation orientation, LabelFilter &labelFilter);

    void loadLabelTree(Symbol &symbol) const;
    void appendSubItems(LabelTreeItem *parent, QStringList &labels) const;

    QPair<double, double> equationBounds(int row);

    QVariant specialValue(double value);
    QVariant specialValueMinMax(double value, Qt::Orientation orientation);
    QVariant specialMarginalValue(double value);
    QVariant specialMarginalEquValueBasis(double value, int rIndex);
    QVariant specialMarginalVarValueBasis(double value, int cIndex);

    static int errorCallback(int count, const char *message);

private:
    class SymbolCache;
    SymbolCache *mSymbolCache;

    DataHandler *mDataHandler;

    gevHandle_t mGEV = nullptr;
    gmoHandle_t mGMO = nullptr;
    dctHandle_t mDCT = nullptr;
    std::function<QVariant(double, int)> specialMarginalEquValuePtr;
    std::function<QVariant(double, int)> specialMarginalVarValuePtr;
};

}
}
}

#endif // MODELINSTANCE_H
