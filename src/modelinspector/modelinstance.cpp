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

#include <QAbstractItemModel>
#include <QDir>
#include <QVector>

#include <QDebug>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance::Cache
{
public:
    Cache(ModelInstance* const modelInstance)
        : mModelInstance(modelInstance)
    {

    }

    void loadSymbols(int type)
    {
        int sectionIndex = PredefinedHeaderLength;
        QVector<SymbolInfo> &syms = (type == dcteqnSymType) ? mEquations : mVariables;
        for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
            auto sym = mModelInstance->loadSymbol(i, sectionIndex);
            if (type != sym.Type)
                continue;
            sectionIndex += sym.Entries;
            syms.append(sym);
            if (type == dcteqnSymType)
                mEquationEntries += sym.Entries;
            else
                mVariableEntries += sym.Entries;
        }
    }

    void loadJaccobian()
    {
        for (int r=0; r<mModelInstance->equationCount(); ++r) {
            mJaccobian.push_back(mModelInstance->jaccobianRow(r));
        }
    }

    void loadHorizontalAttributes()
    {
        for (int r=0; r<PredefinedHeaderLength; ++r) {
            auto header = PredefinedHeader.at(r).toLower();
            for (int c=0; c<mModelInstance->columnCount()-PredefinedHeaderLength; ++c) {
                mHorizontalAttributes[r][c] = mModelInstance->horizontalAttribute(header, c);
            }
        }
    }

    void loadVerticalAttributes()
    {
        for (int c=0; c<PredefinedHeaderLength; ++c) {
            auto header = PredefinedHeader.at(c).toLower();
            for (int r=0; r<mModelInstance->rowCount()-PredefinedHeaderLength; ++r) {
                mVerticalAttributes[c][r] = mModelInstance->verticalAttribute(header, r);
            }
        }
    }

    void loadHorizontalHeaderData()
    {
        int itemIndex = PredefinedHeaderLength;
        Q_FOREACH(const auto &var, symbols(dctvarSymType)) {
            HMaxSymbolDimension = qMax(HMaxSymbolDimension, var.Dimension);
            for (int i=itemIndex; i<itemIndex+var.Entries; ++i) {
                HSectionIndexToSymbol[i] = var;
            }
            itemIndex += var.Entries;
        }
    }

    void loadVerticalHeaderData()
    {
        int itemIndex = PredefinedHeaderLength;
        Q_FOREACH(const auto &eqn, symbols(dcteqnSymType)) {
            VMaxSymbolDimension = qMax(VMaxSymbolDimension, eqn.Dimension);
            for (int i=itemIndex; i<itemIndex+eqn.Entries; ++i) {
                VSectionIndexToSymbol[i] = eqn;
            }
            itemIndex += eqn.Entries;
        }
    }

    QVariant jaccobianValue(int column, int row) const
    {
        const static QVariant defaultValue(0.0);
        if (row < mJaccobian.size())
            return mJaccobian[row].value(column);
        return defaultValue;
    }

    QVariant horizontalAttribute(int row, int column) const
    {
        return mHorizontalAttributes[row][column];
    }

    QVariant verticalAttribute(int row, int column) const
    {
        return mVerticalAttributes[column][row];
    }

    int symbolEntries(int type) const {
        return type == dcteqnSymType ? mEquationEntries : mVariableEntries;
    }

    const QVector<SymbolInfo>& symbols(int type) const
    {
        return type == dcteqnSymType ? mEquations : mVariables;
    }

    ValueFilter& initalValueFilter()
    {
        return mInitialValueFilter;
    }

    QString longestEqnText() {
        if (mLongestEqnText.isEmpty()) {
            Q_FOREACH(auto symbol, mModelInstance->equations()) {
                if (symbol.Name.size() > mLongestEqnText.size())
                    mLongestEqnText = symbol.Name;
            }
        }
        return mLongestEqnText;
    }

    QString longestEqnUelText() {
        if (mLongestEqnUelText.isEmpty()) {
            Q_FOREACH(auto uel, InitialUelFilter[Qt::Vertical].keys()) {
                if (uel.size() > mLongestEqnUelText.size())
                    mLongestEqnUelText = uel;
            }
        }
        return mLongestEqnUelText;
    }

    QString longestVarText() {
        if (mLongestVarText.isEmpty()) {
            Q_FOREACH(auto symbol, mModelInstance->variables()) {
                if (symbol.Name.size() > mLongestVarText.size())
                    mLongestVarText = symbol.Name;
            }
        }
        return mLongestVarText;
    }

    QString longestVarUelText() {
        if (mLongestVarUelText.isEmpty()) {
            Q_FOREACH(auto uel, InitialUelFilter[Qt::Horizontal].keys()) {
                if (uel.size() > mLongestVarUelText.size())
                    mLongestVarUelText = uel;
            }
        }
        return mLongestVarUelText;
    }

    const SymbolInfo& sectionSymbol(int sectionIndex, Qt::Orientation orientation)
    {
        if (orientation == Qt::Horizontal)
            return HSectionIndexToSymbol[sectionIndex];
        return VSectionIndexToSymbol[sectionIndex];
    }

public:
    QMap<int, SymbolInfo> HSectionIndexToSymbol;
    QMap<int, SymbolInfo> VSectionIndexToSymbol;
    int HMaxSymbolDimension = 0;
    int VMaxSymbolDimension = 0;

    UelFilterMap  InitialUelFilter;

private:
    ModelInstance* const mModelInstance;

    QVector<SymbolInfo> mEquations;
    QVector<SymbolInfo> mVariables;

    int mEquationEntries = 0;
    int mVariableEntries = 0;

    ValueFilter mInitialValueFilter;

    QString mLongestEqnText;
    QString mLongestEqnUelText;

    QString mLongestVarText;
    QString mLongestVarUelText;

    AttributeValues mHorizontalAttributes;
    AttributeValues mVerticalAttributes;

    Jaccobian mJaccobian;
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
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int positiveCoeffs = 0;
    for (int row=0; row<equationCount(); ++row) {
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
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int negativeCoeffs = 0;
    for (int row=0; row<equationCount(); ++row) {
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

SymbolInfo ModelInstance::equation(int sectionIndex) const
{
    return mCache->VSectionIndexToSymbol[sectionIndex];
}

const QVector<SymbolInfo>& ModelInstance::equations() const
{
    return mCache->symbols(dcteqnSymType);
}

int ModelInstance::equationCount() const
{
    return gmoM(mGMO);
}

int ModelInstance::equationCount(int type) const
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

QString ModelInstance::longestEqnText() const
{
    auto eqn = mCache->longestEqnText();
    auto uel = mCache->longestEqnUelText();
    if (eqn.size() > uel.size())
        return eqn;
    return uel;
}

QString ModelInstance::longestVarText() const
{
    auto var = mCache->longestVarText();
    auto uel = mCache->longestVarUelText();
    if (var.size() > uel.size())
        return var;
    return uel;
}

SymbolInfo ModelInstance::variable(int sectionIndex) const
{
    return mCache->HSectionIndexToSymbol[sectionIndex];
}

const QVector<SymbolInfo>& ModelInstance::variables() const
{
    return mCache->symbols(dctvarSymType);
}

int ModelInstance::variableCount() const
{
    return gmoN(mGMO);
}

int ModelInstance::maxEquationDimension() const
{
    return mCache->HMaxSymbolDimension;
}

int ModelInstance::maxVariableDimension() const
{
    return mCache->VMaxSymbolDimension;
}

int ModelInstance::variableCount(int type) const
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

void ModelInstance::loadTableData()
{
    mCache->loadSymbols(dcteqnSymType);
    mCache->loadSymbols(dctvarSymType);
    loadInitialUelFilter(Qt::Horizontal);
    loadInitialUelFilter(Qt::Vertical);
    mCache->loadHorizontalAttributes();
    mCache->loadVerticalAttributes();
    mCache->loadJaccobian();
    mCache->loadHorizontalHeaderData();
    mCache->loadVerticalHeaderData();
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
    mCache->initalValueFilter().MinValue = range.first;
    mCache->initalValueFilter().MaxValue = range.second;
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

SymbolInfo ModelInstance::loadSymbol(int index, int sectionIndex) const
{
    SymbolInfo info;
    if (index > symbolCount())
        return info;

    info.Index = index;
    info.setFirstSection(sectionIndex);
    info.Offset = dctSymOffset(mDCT, index);
    info.Dimension = dctSymDim(mDCT, index);
    info.Entries = dctSymEntries(mDCT, index);

    char symbolName[GMS_SSSIZE];
    if (dctSymName(mDCT, index, symbolName, GMS_SSSIZE))
        info.Name = "ERROR";
    else
        info.Name = symbolName;

    info.Type = dctSymType(mDCT, index);
    loadDimensions(info);

    return info;
}

void ModelInstance::loadDimensions(SymbolInfo &symbol) const
{
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbol.Entries; ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbol.Offset + j) < 0)
            continue;

        int symIndex;
        if (symbol.Type == dctvarSymType) {
            if (dctColUels(mDCT, symbol.Offset+j, &symIndex, domains, &nDomains))
                continue;
        } else {
            if (dctRowUels(mDCT, symbol.Offset+j, &symIndex, domains, &nDomains))
                continue;
        }

        char quote;
        char uelName[GMS_SSSIZE];
        QStringList uels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, uelName, GMS_SSSIZE);
            uels << uelName;
        }
        symbol.DimensionData[symbol.firstSection()+j] = uels;
    }
}

const SymbolInfo& ModelInstance::symbol(int index, int type) const
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
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equationCount(); ++row) {
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
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equationCount(); ++row) {
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
    auto columns = variableCount();

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
    auto rows = equationCount();
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

int ModelInstance::rowCount() const
{
    return PredefinedHeaderLength + mCache->symbolEntries(dcteqnSymType);
}

int ModelInstance::columnCount() const
{
    return PredefinedHeaderLength + mCache->symbolEntries(dctvarSymType);
}

void ModelInstance::loadInitialUelFilter(Qt::Orientation orientation)
{
    UelFilter filter;
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
            char name[GMS_SSSIZE];
            for (int k=0; k<nDomains; ++k) {
                dctUelLabel(mDCT, domains[k], &quote, name, GMS_SSSIZE);
                filter[name] = Qt::Checked;
            }
        }
    }
    mCache->InitialUelFilter[orientation] = filter;
}

QVariant ModelInstance::data(int row, int column) const
{
    int cIndex =  column - PredefinedHeaderLength;
    if (row < PredefinedHeaderLength) {
        return mCache->horizontalAttribute(row, cIndex);
    }

    int rIndex = row - PredefinedHeaderLength;
    if (column < PredefinedHeaderLength) {
        return mCache->verticalAttribute(rIndex, column);
    }

    return mCache->jaccobianValue(cIndex, rIndex);
}

QString ModelInstance::headerData(int sectionIndex, int dimension, Qt::Orientation orientation) const
{
    if (sectionIndex < PredefinedHeaderLength)
        return PredefinedHeader.at(sectionIndex);
    if (orientation == Qt::Horizontal) {
        auto var = variable(sectionIndex);
        if (dimension < 0)
            return var.Name;
        return var.dimensionData(sectionIndex, dimension);
    }
    auto eqn = equation(sectionIndex);
    if (dimension < 0)
        return eqn.Name;
    return eqn.dimensionData(sectionIndex, dimension);
}

void ModelInstance::searchSymbolData(int logicalIndex,
                                     int sectionIndex,
                                     const QString &term,
                                     bool isRegEx,
                                     Qt::Orientation orientation,
                                     QList<SearchResult> &result)
{
    std::function<bool(const QString&)> compare;
    if (isRegEx) {
        QRegExp regex(term);
        compare = [regex](const QString &text) {
            return regex.exactMatch(text);
        };
    } else {
        compare = [&term](const QString &text) {
            return text.contains(term, Qt::CaseInsensitive);
        };
    }

    auto sym = mCache->sectionSymbol(sectionIndex, orientation);
    if (compare(sym.Name)) {
        result.append(SearchResult{logicalIndex, orientation});
    } else {
        auto uels = sym.DimensionData[sectionIndex];
        Q_FOREACH(const auto uel, uels) {
            if (compare(uel)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

SymbolFilter ModelInstance::initialSymboldFilter(QAbstractItemModel *model,
                                                 Qt::Orientation orientation)
{
    int sections = orientation == Qt::Horizontal ? model->columnCount() :
                                                   model->rowCount();
    bool ok;
    SymbolFilter filter;
    QSet<QString> symNames;
    for (int s=0; s<PredefinedHeaderLength; ++s) {
        auto data = headerData(s, -1, orientation);
        filter.push_back(SymbolFilterItem{ false, -1, data, Qt::Unchecked });
    }
    for (int s=0; s<sections; ++s) {
        int realSection = model->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        auto data = headerData(realSection, -1, orientation);
        if (realSection < PredefinedHeaderLength) {
            filter[realSection] = SymbolFilterItem{ true, realSection, data, Qt::Checked };
        } else if (!symNames.contains(data)) {
            symNames.insert(data);
            filter.push_back(SymbolFilterItem{ true, realSection, data, Qt::Checked });
        }
    }
    return filter;
}

UelFilterMap ModelInstance::initialUelFilter() const
{
    return mCache->InitialUelFilter;
}

ValueFilter ModelInstance::initalValueFilter() const
{
    return mCache->initalValueFilter();
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
                    sizeof(msg))) {
        mLogMessages << "ERROR: " + QString(msg);
    }
}

JaccobianRow ModelInstance::jaccobianRow(int row)
{
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    JaccobianRow jacRow;
    if (!gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz)) {
        for (int i=0; i<nz; ++i) {
            jacRow[colidx[i]] = QVariant(jacval[i]);
        }
    }

    delete [] colidx;
    delete [] jacval;
    delete [] nlflag;
    return jacRow;
}

QVariant ModelInstance::horizontalAttribute(const QString &header, int column)
{
    double value = 0.0;
    if (header == "level") {
        value = gmoGetVarLOne(mGMO, column);
    } else if (header == "lower") {
        value = gmoGetVarLowerOne(mGMO, column);
    } else if (header == "marginal") {
        value = gmoGetVarMOne(mGMO, column);
        return specialMarginalVarValuePtr(value, column);
    } else if (header == "scale") {
        value = gmoGetVarScaleOne(mGMO, column);
    } else if (header == "upper") {
        value = gmoGetVarUpperOne(mGMO, column);
    }
    return specialValue(value);
}

QVariant ModelInstance::verticalAttribute(const QString &header, int row)
{
    double value = 0.0;
    if (header == "level") {
        value = gmoGetEquLOne(mGMO, row);
    } else if (header == "lower") {
        auto bounds = equationBounds(row);
        value = bounds.first;
    } else if (header == "marginal") {
        value = gmoGetEquMOne(mGMO, row);
        return specialMarginalEquValuePtr(value, row);
    } else if (header == "scale") {
        value = gmoGetEquScaleOne(mGMO, row);
    } else if (header == "upper") {
        auto bounds = equationBounds(row);
        value = bounds.second;
    }
    return specialValue(value);
}

QPair<double, double> ModelInstance::equationBounds(int row)
{
    QPair<double, double> bounds(-1, -1); // TODO (AF) keep these default values?

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

int ModelInstance::errorCallback(int count, const char *message)
{// TODO (AF) use system logger when integrated into studio
    Q_UNUSED(count)
    qDebug()<< message;
    return 0;
}

}
}
}
