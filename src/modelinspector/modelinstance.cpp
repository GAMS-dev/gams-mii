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

ModelInstance::ModelInstance(const QString &workspace, const QString &scratchDir)
    : mScratchDir(scratchDir)
    , mWorkspace(QDir(workspace).absolutePath())
{
    initialize();
}

ModelInstance::ModelInstance(const ModelInstance &modelInstance)
{
    *this = modelInstance;
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    if (mDCT) dctFree(&mDCT);
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

// TODO (AF) get full jac right away... large model issue?
int ModelInstance::positiveCoefficents() const
{// TODO (AF) optimize... save value
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
{// TODO (AF) optimize... save value
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

int ModelInstance::equationBlocks() const
{// TODO use symbols(dcteqntype)?
    int blocks = 0;
    for (int i=1; i<=symbolCount(); ++i) {
        auto sym = symbol(i);
        if (isEquation(sym.Type))
            ++blocks;
    }
    return blocks;
}

bool ModelInstance::isEquation(int symType) const
{
    return symType == dcteqnSymType ? true : false;
}

int ModelInstance::variables() const
{
    return gmoN(mGMO);
}

int ModelInstance::variables(int type) const
{
    return gmoGetVarTypeCnt(mGMO, type);
}

int ModelInstance::variableBlocks() const
{// TODO user symbols(dctvartype)?
    int blocks = 0;
    for (int i=1; i<=symbolCount(); ++i) {
        auto sym = symbol(i);
        if (isVariable(sym.Type))
            ++blocks;
    }
    return blocks;
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

    // TODO specifc error message that model insp. needs the dct... because it
    //      can be switched off by the user
    // TODO check if gmodict() can be used and this could be droped...?
    QString dictFile = mWorkspace + "/" + mScratchDir + "/gamsdict.dat";
    mLogMessages << "DCT File: " + dictFile;
    if (dctLoadEx(mDCT, dictFile.toStdString().c_str(), msg, sizeof(msg))) {
        mLogMessages << "ERROR: Could not load dictionary file. " + QString(msg);
        return;
    }

    mLogMessages << "Absolute Scrach Path: " + mScratchDir;
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

QVector<QVariant> ModelInstance::variableData(const SymbolInfo &symbol)
{
    int *colidx = new int[gmoN(mGMO)];
    int *nlflag = new int[gmoN(mGMO)];
    double *jacval = new double[gmoN(mGMO)];
    int nnz, nlnnz;

    QVector<QVariant> data;
    int rowidx = rowIndex(symbol.Offset);
    if (rowidx >= 0 && !gmoGetRowSparse(mGMO, rowidx, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        Q_FOREACH(const auto& var, symbols(dctvarSymType)) {
            double min = gmoPinf(mGMO);
            double max = gmoMinf(mGMO);
            bool listed = false;
            for (int i=0; i<nnz; ++i) { // TODO is colidx always ordered by symbol
                if (colidx[i] >= var.Offset && colidx[i] < var.lastOffset()) {
                    listed = true;
                    if (i == 0) {
                        min = jacval[i];
                        max = jacval[i];
                    } else {
                        min = std::min(min, jacval[i]);
                        max = std::max(max, jacval[i]);
                    }
                }
            }

            if (!listed) data.append("");
            else if (max <= 0) data.append("-");
            else if (min >= 0) data.append("+");
            else if (min < 0 && max > 0) data.append("u");
            else data.append("0");
        }
    }

    delete [] colidx;
    delete [] nlflag;
    delete [] jacval;

    return data;
}

QVector<QVariant> ModelInstance::variableType()
{
    QVector<QVariant> types;
    Q_FOREACH(const auto& eqn, symbols(dcteqnSymType)) {
        if (types.isEmpty()) {
            types.append(variableData(eqn));
        } else {
            auto tmp = variableData(eqn);
            for (int i=0; i<types.size(); ++i) {
                if (types[i] == "u")
                if (types[i] == tmp[i])
                    continue;
                if (tmp[i].toString().isEmpty())
                    continue;
                if (types[i].toString().isEmpty())
                    types[i] = tmp[i];
            }
        }
    }
    return types;
}

//QVector<QString> ModelInstance::rowUels(int index, int offset) const
//{// TODO keep?
//    char q;
//    char label[GMS_SSSIZE];
//    QVector<QString> uels;
//    int entries = dctSymEntries(mDCT, index);
//    for (int i=0; i<entries; ++i) {
//        if (dctUelLabel(mDCT, offset+i, &q, label, GMS_SSSIZE))
//            continue;
//        uels.append(label);
//    }
//    return uels;
//}

int ModelInstance::rowIndex(int offset) const
{
    return gmoGetjSolverQuiet(mGMO, offset);
}

//QString ModelInstance::columnUels(int symbolIndex) const
//{ // TODO keep?
//    int symDim = 0,  symIndex = 0;
//    int uelIdcs[GMS_MAX_INDEX_DIM];
//    if (dctRowUels(mDCT, symbolIndex, &symIndex, uelIdcs, &symDim))
//        return QString();
//    char q;
//    char label[GMS_SSSIZE];
//    if (dctUelLabel(mDCT, symIndex, &q, label, GMS_SSSIZE))
//        return QString();
//    return label;
//}

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

SymbolInfo ModelInstance::symbol(int index, int type) const
{// TODO preload/lazy loading symbols?
    auto syms = symbols(type);
    return syms.at(index);
}


QVector<SymbolInfo> ModelInstance::symbols(int typeFilter) const
{
    QVector<SymbolInfo> infos;
    for (int i=1; i<=symbolCount(); ++i) {
        auto sym = symbol(i);
        if (typeFilter < 0)
            infos.append(sym);
        else if (sym.Type == typeFilter)
            infos.append(sym);
    }
    return infos;
}

ModelInstance& ModelInstance::operator=(const ModelInstance &modelInstance)
{
    mScratchDir = modelInstance.mScratchDir;
    mWorkspace = modelInstance.mWorkspace;
    this->initialize();
    return *this;
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

    // TODO filter zielfunktion?
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

int ModelInstance::rowCount() const
{
    int entries = 0;
    Q_FOREACH(const auto sym, symbols(dcteqnSymType)) {
        entries += sym.Entries;
    }
    return PredefinedHeader.size() + entries;
}

int ModelInstance::columnCount() const
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

    Q_FOREACH(const auto &predefined, PredefinedHeader) {
        rootItems.append(new QStandardItem(predefined));
    }
    Q_FOREACH(const auto &var, symbols(dctvarSymType)) {
        auto rootItem = new QStandardItem(var.Name);
        rootItems.append(rootItem);
        appendHeaderColumns(rootItem, var, Qt::Horizontal);
    }

    model.insertRow(0, rootItems);
}

void ModelInstance::verticalHeaderData(QStandardItemModel &model)
{
    QList<QStandardItem*> rootItems;

    Q_FOREACH(const auto &predefined, PredefinedHeader) {
        rootItems.append(new QStandardItem(predefined));
    }
    Q_FOREACH(const auto &eqn, symbols(dcteqnSymType)) {
        auto rootItem = new QStandardItem(eqn.Name);
        rootItems.append(rootItem);
        appendHeaderColumns(rootItem, eqn, Qt::Vertical);
    }

    model.insertRow(0, rootItems);
}

QVariant ModelInstance::data(int row, int column)
{
    if (row < 5 && column < 5) {
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

    // TODO @lutz use the special value stuff here or just the 0 test?
    double value = jaccobianValue(rIndex, cIndex);
    return value != 0.0 ? QVariant(value) : QString();
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
{// TODO check if this approach is correct (talk to lutz)...
 //      probaly the discussion approach is much faster
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    int entries = dctSymEntries(mDCT, symbolInfo.Index);
    for (int j=0; j<entries; ++j) {
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

int ModelInstance::errorCallback(int count, const char *message)
{// TODO
    Q_UNUSED(count)
    //emit newLogMessage("ERROR CALLBACK: " + QString(message));
    qDebug()<< message;
    return 0;
}

}
}
}
