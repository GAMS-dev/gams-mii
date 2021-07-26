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
#include "modelinstance.h"
#include "commonpaths.h"
#include "valuefiltersettings.h"

#include <QDir>
#include <QStandardItem>
#include <QVector>

#include <QDebug>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

const QStringList ModelInstance::PredefinedHeader { "level", "lower", "marginal", "scale", "upper" };
const int ModelInstance::PredefinedHeaderLength = ModelInstance::PredefinedHeader.size();

class ModelInstance::Cache
{
public:
    Cache(ModelInstance* const modelInstance)
        : mModelInstance(modelInstance)
    {

    }

    void loadSymbols(int type)
    {
        QVector<SymbolInfo> &syms = (type == dcteqnSymType) ? mEquations : mVariables;
        for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
            auto sym = mModelInstance->symbol(i);
            if (type == sym.Type)
                syms.append(sym);
        }
    }

    const QVector<SymbolInfo>& symbols(int type) const
    {
        return (type == dcteqnSymType) ? mEquations : mVariables;
    }

public:
    ///
    /// \brief mHIndexToItem Horizontal logical index to QStandardItem mapping.
    ///
    QMap<int, QStandardItem*> HIndexToItem;

    ///
    /// \brief mHItemToIndex QStandardItem to Horizontal logical index mapping.
    ///
    QMap<QStandardItem*, int> HItemToIndex;

    ///
    /// \brief mVIndexToItem Vertical logical index to QStandardItem mapping.
    ///
    QMap<int, QStandardItem*> VIndexToItem;

    ///
    /// \brief mVIndexToItem QStandardItem to Vertical logical index mapping.
    ///
    QMap<QStandardItem*, int> VItemToIndex;

    QList<QStandardItem*> HorizontalSymbols;
    QList<QStandardItem*> VerticalSymbols;

    QMap<QString, bool> HorizontalUelStates;
    QMap<QString, bool> VerticalUelStates;

    ValueFilterSettings ValueFilterSettings;

private:
    ModelInstance* const mModelInstance;

    QVector<SymbolInfo> mEquations;
    QVector<SymbolInfo> mVariables;
};

ModelInstance::ModelInstance(const QString &workspace, const QString &scratchDir)
    : mCache(new Cache(this))
    , mScratchDir(scratchDir)
    , mWorkspace(QDir(workspace).absolutePath())
{
    initialize();
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    if (mDCT) dctFree(&mDCT);
    delete mCache;
}

QString ModelInstance::modelName() const
{
    char name[GMS_SSSIZE];
    gmoNameModel(mGMO, name);
    return name;
}

int ModelInstance::coefficents() const
{
    return gmoNZ(mGMO);
}

int ModelInstance::positiveCoefficents() const
{
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int positiveCoeffs = 0;
    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz);
        for (int idx = 0; idx<nz+nlnz; ++idx) {
            if (jacval[idx] >= 0)
                ++positiveCoeffs;
        }
    }
    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    return positiveCoeffs;
}

int ModelInstance::negativeCoefficents() const
{
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int negativeCoeffs = 0;
    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz);
        for (int idx = 0; idx<nz+nlnz; ++idx) {
            if (jacval[idx] < 0)
                ++negativeCoeffs;
        }
    }
    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    return negativeCoeffs;
}

int ModelInstance::nonLinearCoefficents() const
{
    return gmoNLNZ(mGMO);
}

int ModelInstance::equations() const
{
    return gmoM(mGMO);
}

int ModelInstance::equations(int type) const
{
    return gmoGetEquTypeCnt(mGMO, type);
}

int ModelInstance::equationBlocks()
{
    return symbols(dcteqnSymType).count();
}

bool ModelInstance::isEquation(int symType)
{
    return symType == dcteqnSymType ? true : false;
}

bool ModelInstance::isEquation(const QString &name)
{
    Q_FOREACH(const auto& sym, symbols(dcteqnSymType)) {
        if (sym.Name == name)
            return true;
    }
    return false;
}

int ModelInstance::variables() const
{
    return gmoN(mGMO);
}

int ModelInstance::variables(int type) const
{
    return gmoGetVarTypeCnt(mGMO, type);
}

int ModelInstance::variableBlocks()
{
    return symbols(dctvarSymType).count();
}

bool ModelInstance::isVariable(const QString &name)
{
    Q_FOREACH(const auto& sym, symbols(dctvarSymType)) {
        if (sym.Name == name)
            return true;
    }
    return false;
}

void ModelInstance::loadScratchData()
{
    mLogMessages << "Model Workspace: " + mWorkspace;
    QString ctrlFile = mWorkspace + "/" + mScratchDir + "/gamscntr.dat";
    mLogMessages << "CTRL File: " + ctrlFile;
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str())) {
        mLogMessages << "ERROR: Could not initialize model instance";
        return;
    }

    char msg[GMS_SSSIZE];
    gmoRegisterEnvironment(mGMO, mGEV, msg);
    if (gmoLoadDataLegacy(mGMO, msg)) {
        mLogMessages << "ERROR: Could not load model instance: " + QString(msg);
        return;
    }

    // TODO (AF/LW) specifc error message that model insp. needs the dct... because it
    //      can be switched off by the user
    // TODO (AF/LW) check if gmodict() can be used to drop this?
    QString dictFile = mWorkspace + "/" + mScratchDir + "/gamsdict.dat";
    mLogMessages << "DCT File: " + dictFile;
    if (dctLoadEx(mDCT, dictFile.toStdString().c_str(), msg, sizeof(msg))) {
        mLogMessages << "ERROR: Could not load dictionary file. " + QString(msg);
        return;
    }

    mLogMessages << "Absolute Scrach Path: " + mScratchDir;
}

void ModelInstance::loadSymbols()
{
    mCache->loadSymbols(dcteqnSymType);
    mCache->loadSymbols(dctvarSymType);
}

void ModelInstance::loadMinMaxValues()
{
    QPair<double, double> range { 0.0, 0.0 };
    bool ok;
    for (int r=0; r<rowCount(); ++r) {
        for (int c=0; c<columnCount(); ++c) {
            auto variant = data(r,c);
            if (variant.toString().contains("inf", Qt::CaseInsensitive) ||
                    variant.toString().contains("eps", Qt::CaseInsensitive))
                continue;
            double value = variant.toDouble(&ok);
            if (ok) {
                range.first = std::min(range.first, value);
                range.second = std::max(range.second, value);
            }
        }
    }
    mCache->ValueFilterSettings.MinValue = range.first;
    mCache->ValueFilterSettings.MaxValue = range.second;
}

QString ModelInstance::equationType(int offset) const
{
    char type[GMS_SSSIZE];
    gmoGetEquTypeTxt(mGMO, offset, type);
    return type;
}

QVector<QVariant> ModelInstance::scalarEquationData(int offset) const
{
    int *colidx = new int[gmoN(mGMO)];
    int *nlflag = new int[gmoN(mGMO)];
    double *jacval = new double[gmoN(mGMO)];
    int nnz, nlnnz;

    QVector<QVariant> jacvals;
    int rowidx = rowIndex(offset);
    if (rowidx >= 0 && !gmoGetRowSparse(mGMO, rowidx, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        for (int k=0; k<nnz; ++k) {
            jacvals.append(jacval[k]);
        }
    }

    delete [] colidx;
    delete [] nlflag;
    delete [] jacval;

    return jacvals;
}

QMap<int,QVariant> ModelInstance::equationData(int currentRow) const
{
    int *colidx = new int[gmoN(mGMO)];
    int *nlflag = new int[gmoN(mGMO)];
    double *jacval = new double[gmoN(mGMO)];
    int nnz, nlnnz;

    QMap<int,QVariant> jacvals;
    if (gmoGetRowSparse(mGMO, currentRow, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        return jacvals;
    }

    for (int k=0; k<nnz; ++k) {
        jacvals[colidx[k]] = jacval[k];
    }

    delete [] colidx;
    delete [] nlflag;
    delete [] jacval;

    return jacvals;
}

QVector<MaxMin> ModelInstance::equationVariableScaling(const SymbolInfo &symbol)
{
    int *colidx = new int[gmoN(mGMO)];
    int *nlflag = new int[gmoN(mGMO)];
    double *jacval = new double[gmoN(mGMO)];
    int nnz, nlnnz;

    int rowidx = rowIndex(symbol.Offset);
    QVector<MaxMin> scaling;
    if (rowidx >= 0 && !gmoGetRowSparse(mGMO, rowidx, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        Q_FOREACH(const auto& var, symbols(dctvarSymType)) {
            MaxMin maxmin { false, gmoMinf(mGMO), gmoPinf(mGMO) };
            for (int i=0; i<nnz; ++i) {
                if (colidx[i] >= var.Offset && colidx[i] < var.lastOffset()) {
                    maxmin.Valid = true;
                    maxmin.Max = std::max(maxmin.Max, jacval[i]);
                    maxmin.Min = std::min(maxmin.Min, jacval[i]);
                }
            }
            scaling.push_back(maxmin);
        }
    }

    delete [] colidx;
    delete [] nlflag;
    delete [] jacval;

    return scaling;
}

QVector<MaxMin> ModelInstance::totalScaling()
{
    QVector<MaxMin> scaling;
    Q_FOREACH(const auto& eqn, symbols(dcteqnSymType)) {
        if (scaling.isEmpty()) {
            scaling.append(equationVariableScaling(eqn));
        } else {
            auto tmp = equationVariableScaling(eqn);
            for (int i=0; i<scaling.size(); ++i) {
                if (!tmp.at(i).Valid)
                    continue;
                if (scaling.at(i).Valid) {
                    scaling[i].Max = std::max(scaling.at(i).Max, tmp.at(i).Max);
                    scaling[i].Min = std::min(scaling.at(i).Min, tmp.at(i).Min);
                } else {
                    scaling[i].Valid = true;
                    scaling[i].Max = tmp.at(i).Max;
                    scaling[i].Min = tmp.at(i).Min;
                }
            }
        }
    }
    return scaling;
}

MaxMin ModelInstance::equationScaling(const SymbolInfo &symbol)
{
    int *colidx = new int[gmoN(mGMO)];
    int *nlflag = new int[gmoN(mGMO)];
    double *jacval = new double[gmoN(mGMO)];
    int nnz, nlnnz;

    MaxMin maxmin { true, gmoMinf(mGMO), gmoPinf(mGMO) };
    int rowidx = rowIndex(symbol.Offset);
    if (rowidx >= 0 && !gmoGetRowSparse(mGMO, rowidx, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        for (int i=0; i<nnz; ++i) {
            maxmin.Max = std::max(maxmin.Max, jacval[i]);
            maxmin.Min = std::min(maxmin.Min, jacval[i]);
        }
    }

    delete [] colidx;
    delete [] nlflag;
    delete [] jacval;

    return maxmin;
}

QPair<double,double> ModelInstance::maxminRhs(int offset, int entries) const
{
    QPair<double,double> maxmin { gmoMinf(mGMO), gmoPinf(mGMO) };
    for (int i=offset; i<offset+entries; ++i) {
        maxmin.first = std::max(maxmin.first, gmoGetRhsOne(mGMO, i));
        maxmin.second = std::min(maxmin.second, gmoGetRhsOne(mGMO, i));
    }
    return maxmin;
}

QPair<double,double> ModelInstance::totalRhs()
{
    QPair<double,double> maxmin { gmoMinf(mGMO), gmoPinf(mGMO) };
    Q_FOREACH(auto const& symbol, symbols(dcteqnSymType)) {
        if (symbol.isScalar())
            continue;
        for (int i=symbol.Offset; i<symbol.Offset+symbol.Entries; ++i) {
            maxmin.first = std::max(maxmin.first, gmoGetRhsOne(mGMO, i));
            maxmin.second = std::min(maxmin.second, gmoGetRhsOne(mGMO, i));
        }
    }
    return maxmin;
}

QString ModelInstance::aggregatedRhs(const SymbolInfo &symbol) const
{
    double min = gmoGetRhsOne(mGMO, symbol.Offset);
    double max = gmoGetRhsOne(mGMO, symbol.Offset);
    if (symbol.Entries) {
        for (int i=1; i<symbol.Entries; ++i) {
            min = std::min(min, gmoGetRhsOne(mGMO, symbol.Offset+i));
            max = std::max(max, gmoGetRhsOne(mGMO, symbol.Offset+i));
        }
    }

    if (max <= 0) return "-";
    if (min >= 0) return "+";
    if (min < 0 && max > 0) return "u";
    return "0";
}

int ModelInstance::rowIndex(int offset) const
{
    return gmoGetjSolverQuiet(mGMO, offset);
}

QStringList ModelInstance::symbolNames() const
{
    QStringList names;
    for (int i=1; i<=symbolCount(); ++i) {
        char name[GMS_SSSIZE];
        if (dctSymName(mDCT, i, name, GMS_SSSIZE))
            continue;
        names << name;
    }
    return names;
}

int ModelInstance::symbolIndex(const QString &label) const
{
    return dctSymIndex(mDCT, label.toLatin1().toStdString().c_str());
}

int ModelInstance::symbolOffset(const QString &label) const
{
    int index = dctSymIndex(mDCT, label.toLatin1().toStdString().c_str());
    if (index <= 0)
        return -1;
    return dctSymOffset(mDCT, index);
}

SymbolInfo ModelInstance::symbol(int index) const
{
    SymbolInfo info;
    if (index > symbolCount())
        return info;

    info.Index = index;
    info.Offset = dctSymOffset(mDCT, index);
    info.Dimension = dctSymDim(mDCT, index);
    info.Entries = dctSymEntries(mDCT, index);

    char symbolName[GMS_SSSIZE];
    if (dctSymName(mDCT, index, symbolName, GMS_SSSIZE))
        info.Name = "ERROR";
    else
        info.Name = symbolName;

    info.Type = dctSymType(mDCT, index);

    return info;
}

SymbolInfo ModelInstance::symbol(int index, int type)
{
    return symbols(type).at(index);
}

const QVector<SymbolInfo>& ModelInstance::symbols(int type) const
{
    return mCache->symbols(type);
}

QPair<double, double> ModelInstance::matrixRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    QPair<double, double> range;
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz);
        for (int idx = 0; idx<nz+nlnz; ++idx) {
            if (row == 0 && idx == 0) {
                range.first = jacval[idx];
                range.second = jacval[idx];
            } else {
                range.first = std::min(range.first, jacval[idx]);
                range.second = std::max(range.second, jacval[idx]);
            }
        }
    }

    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    gmoObjStyleSet(mGMO, gmoObjType_Var);
    return range;
}

QPair<double, double> ModelInstance::objectiveRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    QPair<double, double> range;
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equations(); ++row) {
        gmoGetObjSparse(mGMO, colidx, jacval, nlflag, &nz, &nlnz);
        for (int idx=0; idx<nz+nlnz; ++idx) {
            if (row == 0 && idx == 0) {
                range.first = jacval[idx];
                range.second = jacval[idx];
            } else {
                range.first = std::min(range.first, jacval[idx]);
                range.second = std::max(range.second, jacval[idx]);
            }
        }
    }

    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    gmoObjStyleSet(mGMO, gmoObjType_Var);
    return range;
}

QPair<double, double> ModelInstance::boundsRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    QPair<double, double> range;
    auto columns = variables();

    auto lowerVals = new double[columns];
    if (!gmoGetVarLower(mGMO, lowerVals)) {
        //qDebug() << "gmoGetVarLower";
        for (int i=0; i< columns; ++i) {
            if (i == 0) {
                range.first = lowerVals[i];
            } else {
                range.first = std::min(range.first, lowerVals[i]);
            }
        }
    }

    auto upperVals = new double[columns];
    if (!gmoGetVarUpper(mGMO, upperVals)) {
        //qDebug() << "gmoGetVarUpper";
        for (int i=0; i< columns; ++i) {
            if (i == 0) {
                range.second = upperVals[i];
            } else {
                range.second = std::max(range.second, upperVals[i]);
            }
        }
    }

    delete [] lowerVals;
    delete [] upperVals;
    gmoObjStyleSet(mGMO, gmoObjType_Var);
    return range;
}

QPair<double, double> ModelInstance::rhsRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    QPair<double, double> range;
    auto rows = equations();
    auto *vals = new double[rows];
    if (gmoGetRhs(mGMO, vals))
        return range;
    for (int i=0; i<rows; ++i) {
        if (i == 0) {
            range.first = vals[i];
            range.second = vals[i];
        } else {
            range.first = std::min(range.first, vals[i]);
            range.second = std::max(range.second, vals[i]);
        }
    }

    delete [] vals;
    gmoObjStyleSet(mGMO, gmoObjType_Var);
    return range;
}

int ModelInstance::rowCount()
{
    int entries = 0;
    Q_FOREACH(const auto sym, symbols(dcteqnSymType)) {
        entries += sym.Entries;
    }
    return PredefinedHeader.size() + entries;
}

int ModelInstance::columnCount()
{
    int entries = 0;
    Q_FOREACH(const auto sym, symbols(dctvarSymType)) {
        entries += sym.Entries;
    }
    return PredefinedHeader.size() + entries;
}

void ModelInstance::horizontalHeaderData(QStandardItemModel &model)
{
    QList<QStandardItem*> rootItems;

    for (int i=0; i<PredefinedHeaderLength; ++i) {
        auto rootItem = new QStandardItem(PredefinedHeader.at(i));
        rootItem->setCheckState(Qt::Checked);
        mCache->HorizontalSymbols.push_back(rootItem);
        mCache->HIndexToItem[i] = rootItem;
        if (!rootItem->hasChildren())
            mCache->HItemToIndex[rootItem] = i;
        rootItems.append(rootItem);
    }
    int itemIndex = PredefinedHeaderLength;
    Q_FOREACH(const auto &var, symbols(dctvarSymType)) {
        auto rootItem = new QStandardItem(var.Name);
        rootItem->setCheckState(Qt::Checked);
        mCache->HorizontalSymbols.push_back(rootItem);
        mCache->HItemToIndex[rootItem] = itemIndex;
        for (int i=itemIndex; i<itemIndex+var.Entries; ++i)
            mCache->HIndexToItem[i] = rootItem;
        itemIndex += var.Entries;
        rootItems.append(rootItem);
        appendHeaderColumns(rootItem, var, Qt::Horizontal);
    }

    Q_FOREACH(auto symbol, mCache->HorizontalSymbols) {
        setItemToIndexMapping(symbol, mCache->HItemToIndex);
    }

    model.insertRow(0, rootItems);
}

void ModelInstance::verticalHeaderData(QStandardItemModel &model)
{
    QList<QStandardItem*> rootItems;

    for (int i=0; i<PredefinedHeaderLength; ++i) {
        auto rootItem = new QStandardItem(PredefinedHeader.at(i));
        rootItem->setCheckState(Qt::Checked);
        mCache->VerticalSymbols.push_back(rootItem);
        mCache->VIndexToItem[i] = rootItem;
        if (!rootItem->hasChildren())
            mCache->VItemToIndex[rootItem] = i;
        rootItems.append(rootItem);
    }
    int itemIndex = PredefinedHeaderLength;
    Q_FOREACH(const auto &eqn, symbols(dcteqnSymType)) {
        auto rootItem = new QStandardItem(eqn.Name);
        rootItem->setCheckState(Qt::Checked);
        mCache->VerticalSymbols.push_back(rootItem);
        mCache->VItemToIndex[rootItem] = itemIndex;
        for (int i=itemIndex; i<itemIndex+eqn.Entries; ++i)
            mCache->VIndexToItem[i] = rootItem;
        itemIndex += eqn.Entries;
        rootItems.append(rootItem);
        appendHeaderColumns(rootItem, eqn, Qt::Vertical);
    }

    Q_FOREACH(auto symbol, mCache->VerticalSymbols) {
        setItemToIndexMapping(symbol, mCache->VItemToIndex);
    }

    model.insertRow(0, rootItems);
}

QStandardItem* ModelInstance::horizontalItem(int logicalIndex)
{
    return mCache->HIndexToItem.contains(logicalIndex) ? mCache->HIndexToItem[logicalIndex] :
                                                         nullptr;
}

QStandardItem* ModelInstance::verticalItem(int logicalIndex)
{
    return mCache->VIndexToItem.contains(logicalIndex) ? mCache->VIndexToItem[logicalIndex] :
                                                         nullptr;
}

QList<QStandardItem*> ModelInstance::horizontalItems()
{
    return mCache->HIndexToItem.values();
}

QList<QStandardItem*> ModelInstance::verticalItems()
{
    return mCache->VIndexToItem.values();
}

QList<QStandardItem*> ModelInstance::horizontalUelItems()
{
    return mCache->HItemToIndex.keys();
}

QList<QStandardItem*> ModelInstance::verticalUelItems()
{
    return mCache->VItemToIndex.keys();
}

QList<QStandardItem*> ModelInstance::horizontalSymbols() const
{
    return mCache->HorizontalSymbols;
}

QList<QStandardItem*> ModelInstance::verticalSymbols() const
{
    return mCache->VerticalSymbols;
}

QList<QStandardItem*> ModelInstance::setBranchState(QStandardItem *startItem, Qt::CheckState state)
{
    QList<QStandardItem*> branch;
    QList<QStandardItem*> items { startItem };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        for (int c=0; c<item->columnCount(); ++c)
            items.append(item->child(0, c));
        item->setCheckState(state);
        branch.append(item);
    }
    return branch;
}

void ModelInstance::setUelStates(Qt::Orientation orientation,
                                 const QMap<QString, bool> &uelStates)
{
    if (orientation == Qt::Horizontal)
        mCache->HorizontalUelStates = uelStates;
    mCache->VerticalUelStates = uelStates;
}

QMap<QString, bool> ModelInstance::uelStates(Qt::Orientation orientation)
{
    QMap<QString, bool> &uelStates = orientation == Qt::Horizontal ? mCache->HorizontalUelStates :
                                                                     mCache->VerticalUelStates;
    if (!uelStates.isEmpty()) {
        return uelStates;
    }

    Q_FOREACH(const auto &symbolInfo, symbols(dcteqnSymType)) {
        int nDomains;
        int domains[GLOBAL_MAX_INDEX_DIM];
        for (int j=0; j<symbolInfo.Entries; ++j) {
            if (gmoGetjSolverQuiet(mGMO, symbolInfo.Offset + j) < 0)
                continue;

            int symIndex;
            if (orientation == Qt::Horizontal) {
                if (dctColUels(mDCT, symbolInfo.Offset+j, &symIndex, domains, &nDomains))
                    continue;
            } else {
                if (dctRowUels(mDCT, symbolInfo.Offset+j, &symIndex, domains, &nDomains))
                    continue;
            }

            char quote;
            char uelName[GMS_SSSIZE];
            for (int k=0; k<nDomains; ++k) {
                dctUelLabel(mDCT, domains[k], &quote, uelName, GMS_SSSIZE);
                uelStates[uelName] = true;
            }
        }
    }

    return uelStates;
}

int ModelInstance::horizontalIndex(QStandardItem *item)
{
    return mCache->HItemToIndex.contains(item) ? mCache->HItemToIndex[item] : -1;
}

int ModelInstance::verticalIndex(QStandardItem *item)
{
    return mCache->VItemToIndex.contains(item) ? mCache->VItemToIndex[item] : -1;
}

int ModelInstance::itemToIndex(Qt::Orientation orientation, QStandardItem *item)
{// TODO remove orientation
    int index = -1;
    if (orientation == Qt::Horizontal)
        index = horizontalIndex(item);
    else
        index = verticalIndex(item);
    return index;
}

void ModelInstance::setHeaderRootItemEnabled(QStandardItem *item, bool enabled)
{
    QList<QStandardItem*> items { item };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        for (int c=0; c<item->columnCount(); ++c) {
            items.append(item->child(0, c));
        }
        item->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    }
}

int ModelInstance::predefinedHeaderLength() const
{
    return PredefinedHeaderLength;
}

QVariant ModelInstance::data(int row, int column)
{
    if (row < PredefinedHeaderLength && column < PredefinedHeaderLength) {
        return QVariant();
    }

    int cIndex =  column - PredefinedHeaderLength;
    if (row < PredefinedHeaderLength && column >= PredefinedHeaderLength) {
        return variableAttribute(row, cIndex);
    }

    int rIndex = row - PredefinedHeaderLength;
    if (row >= PredefinedHeaderLength && column < PredefinedHeaderLength) {
        return equationAttribute(column, rIndex);
    }

    double value = jaccobianValue(rIndex, cIndex);
    return value != 0.0 ? QVariant(value) : QString();
}

ValueFilterSettings ModelInstance::valueFilterSettings() const
{
    return mCache->ValueFilterSettings;
}

void ModelInstance::setValueFilterSettings(const ValueFilterSettings &settings)
{
    mCache->ValueFilterSettings = settings;
}

void ModelInstance::initialize()
{
    gevSetExitIndicator(0); // switch of lib exit() call
    gevSetScreenIndicator(0); // switch off std lib output
    gevSetErrorCallback(ModelInstance::errorCallback);

    mLogMessages << "GAMS System Dir: " + CommonPaths::systemDir();

    char msg[GMS_SSSIZE];
    if (!gevCreateD(&mGEV,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);

    gmoSetExitIndicator(0); // switch of lib exit() call
    gmoSetScreenIndicator(0); // switch off std lib output
    gmoSetErrorCallback(ModelInstance::errorCallback);

    if (!gmoCreateD(&mGMO,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);

    if (gmoHaveBasis(mGMO)){
        specialMarginalEquValuePtr = std::bind(&ModelInstance::specialMarginalVarValueBasis,
                                               this, std::placeholders::_1,
                                               std::placeholders::_2);
        specialMarginalVarValuePtr = std::bind(&ModelInstance::specialMarginalVarValueBasis,
                                               this, std::placeholders::_1,
                                               std::placeholders::_2);
    } else {
        specialMarginalEquValuePtr = std::bind(&ModelInstance::specialMarginalValue,
                                               this, std::placeholders::_1);
        specialMarginalVarValuePtr = std::bind(&ModelInstance::specialMarginalValue,
                                               this, std::placeholders::_1);
    }


    dctSetExitIndicator(0); // switch of lib exit() call
    dctSetScreenIndicator(0); // switch off std lib output
    dctSetErrorCallback(ModelInstance::errorCallback);

    if (!dctCreateD(&mDCT,
                    CommonPaths::systemDir().toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);
}

double ModelInstance::jaccobianValue(int row, int column)
{
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    double value = 0.0;
    if (!gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz)) {
        for (int i=0; i<nz; ++i) {
            if (colidx[i] == column) {
                value = jacval[i];
                break;
            }
        }
    }

    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    return value;
}

QVariant ModelInstance::variableAttribute(int row, int cIndex)
{
    double value = 0.0;
    auto header = PredefinedHeader.at(row).toLower();
    if (header == "level") {
        value = gmoGetVarLOne(mGMO, cIndex);
    } else if (header == "lower") {
        value = gmoGetVarLowerOne(mGMO, cIndex);
    } else if (header == "marginal") {
        value = gmoGetVarMOne(mGMO, cIndex);
        return specialMarginalVarValuePtr(value, cIndex);
    } else if (header == "scale") {
        value = gmoGetVarScaleOne(mGMO, cIndex);
    } else if (header == "upper") {
        value = gmoGetVarUpperOne(mGMO, cIndex);
    }
    return specialValue(value);
}

QVariant ModelInstance::equationAttribute(int column, int rIndex)
{
    double value = 0.0;
    auto header = PredefinedHeader.at(column).toLower();
    if (header == "level") {
        value = gmoGetEquLOne(mGMO, rIndex);
    } else if (header == "lower") {
        auto bounds = equationBounds(rIndex);
        value = bounds.first;
    } else if (header == "marginal") {
        value = gmoGetEquMOne(mGMO, rIndex);
        return specialMarginalEquValuePtr(value, rIndex);
    } else if (header == "scale") {
        value = gmoGetEquScaleOne(mGMO, rIndex);
    } else if (header == "upper") {
        auto bounds = equationBounds(rIndex);
        value = bounds.second;
    }
    return specialValue(value);
}

QPair<double, double> ModelInstance::equationBounds(int row)
{
    QPair<double, double> bounds(-1, -1); // TODO keep these default values?

    switch (gmoGetEquTypeOne(mGMO, row)) {
    case gmoequ_B:
    case gmoequ_E:
        bounds.first = gmoGetRhsOne(mGMO, row);
        bounds.second = gmoGetRhsOne(mGMO, row);
        break;
    case gmoequ_C:
    case gmoequ_G:
        bounds.first = gmoGetRhsOne(mGMO, row);
        bounds.second = gmoPinf(mGMO);
        break;
    case gmoequ_L:
        bounds.first = gmoMinf(mGMO);
        bounds.second = gmoGetRhsOne(mGMO, row);
        break;
    case gmoequ_N:
        bounds.first = gmoMinf(mGMO);
        bounds.second = gmoPinf(mGMO);
        break;
    case gmoequ_X:
        bounds.first = 0.0;
        bounds.second = 0.0;
        break;
    }
    return bounds;
}

void ModelInstance::appendHeaderColumns(QStandardItem *parent,
                                        const SymbolInfo &symbolInfo,
                                        Qt::Orientation orientation)
{// TODO check later if we want to improve performance... non-recursive
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbolInfo.Entries; ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbolInfo.Offset + j) < 0)
            continue;

        int symIndex;
        if (orientation == Qt::Horizontal) {
            if (dctColUels(mDCT, symbolInfo.Offset+j, &symIndex, domains, &nDomains))
                continue;
        } else {
            if (dctRowUels(mDCT, symbolInfo.Offset+j, &symIndex, domains, &nDomains))
                continue;
        }

        char quote;
        char uelName[GMS_SSSIZE];
        QStringList uels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, uelName, GMS_SSSIZE);
            uels << uelName;
        }

        appendHeaderItems(parent, uels);
    }
}

void ModelInstance::appendHeaderItems(QStandardItem *parent, QStringList &uels)
{
    if (uels.isEmpty())
        return;
    auto uel = uels.takeFirst();
    if (parent->columnCount() == 0) {
        auto child = new QStandardItem(uel);
        child->setCheckState(Qt::Checked);
        parent->appendColumn({child});
        appendHeaderItems(child, uels);
    } else {
        bool found = false;
        for (int i=0; i<parent->columnCount(); ++i) {
            auto child = parent->child(0, i);
            if (child->text() == uel) {
                found = true;
                appendHeaderItems(child, uels);
                break;
            }
        }
        if (!found) {
            auto child = new QStandardItem(uel);
            child->setCheckState(Qt::Checked);
            parent->appendColumn({child});
            appendHeaderItems(child, uels);
        }
    }
}

QVariant ModelInstance::specialValue(double value)
{
    if (value == 0.0)
        return QVariant();
    else if (gmoPinf(mGMO) == value)
        return "+INF";
    else if (gmoMinf(mGMO) == value)
        return "-INF";
    return value;
}

QVariant ModelInstance::specialMarginalValue(double value)
{
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalEquValueBasis(double value, int rIndex)
{
    if (gmoGetEquStatOne(mGMO, rIndex) != gmoBstat_Basic && value == 0.0)
        return "EPS";
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalVarValueBasis(double value, int cIndex)
{
    if (gmoGetVarStatOne(mGMO, cIndex) != gmoBstat_Basic && value == 0.0)
        return "EPS";
    return specialValue(value);
}

void ModelInstance::setItemToIndexMapping(QStandardItem *item, QMap<QStandardItem *, int> &mapping)
{
    auto parent = item;
    while (parent->parent())
        parent = parent->parent();

    int parentIndex = mapping[parent];
    QList<QStandardItem*> nodes;
    QList<QStandardItem*> subTree { parent };
    while (!subTree.isEmpty()) {
        auto subItem = subTree.takeFirst();
        if (subItem->columnCount()) {
            nodes.prepend(subItem);
            for (int c=subItem->columnCount()-1; c>=0; --c)
                subTree.prepend(subItem->child(0, c));
        } else {
            mapping[subItem] = parentIndex++;
        }
    }

    Q_FOREACH(auto node, nodes) {
        auto child = node->child(0, 0);
        mapping[node] = mapping[child];
    }
}

int ModelInstance::errorCallback(int count, const char *message)
{// TODO (AF) use system logger when integrated into studio
    Q_UNUSED(count)
    qDebug()<< message;
    return 0;
}

}
}
}
