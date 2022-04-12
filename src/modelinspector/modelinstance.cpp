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
#include "datahandler.h"
#include "filtertreeitem.h"
#include "labeltreeitem.h"

#include <QAbstractItemModel>
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
        int &symEntries = (type == dcteqnSymType) ? mEquationEntries : mVariableEntries;
        QVector<SymbolInfo> &syms = (type == dcteqnSymType) ? mEquations : mVariables;
        for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
            auto sym = mModelInstance->loadSymbol(i, sectionIndex);
            if (type != sym.type())
                continue;
            sectionIndex += sym.entries();
            syms.append(sym);
            symEntries += sym.entries();
        }
    }

    void loadHorizontalHeaderData()
    {
        int itemIndex = PredefinedHeaderLength;
        Q_FOREACH(const auto &var, symbols(dctvarSymType)) {
            MaxVariableDimension = qMax(MaxVariableDimension, var.dimension());
            for (int i=itemIndex; i<itemIndex+var.entries(); ++i) {
                hSectionIndexToSymbol[i] = var;
            }
            itemIndex += var.entries();
        }
    }

    void loadVerticalHeaderData()
    {
        int itemIndex = PredefinedHeaderLength;
        Q_FOREACH(const auto &eqn, symbols(dcteqnSymType)) {
            MaxEquationDimension = qMax(MaxEquationDimension, eqn.dimension());
            for (int i=itemIndex; i<itemIndex+eqn.entries(); ++i) {
                vSectionIndexToSymbol[i] = eqn;
            }
            itemIndex += eqn.entries();
        }
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
                if (symbol.name().size() > mLongestEqnText.size())
                    mLongestEqnText = symbol.name();
            }
        }
        return mLongestEqnText;
    }

    QString longestEqnLabelText() {
        if (mLongestEqnLabelText.isEmpty()) {
            for (auto iter=InitialLabelFilter.LabelCheckStates[Qt::Vertical].constKeyValueBegin();
                 iter!=InitialLabelFilter.LabelCheckStates[Qt::Vertical].constKeyValueEnd(); ++iter) {
                if (iter->first.size() > mLongestEqnLabelText.size())
                    mLongestEqnLabelText = iter->first;
            }
        }
        return mLongestEqnLabelText;
    }

    QString longestVarText() {
        if (mLongestVarText.isEmpty()) {
            Q_FOREACH(auto symbol, mModelInstance->variables()) {
                if (symbol.name().size() > mLongestVarText.size())
                    mLongestVarText = symbol.name();
            }
        }
        return mLongestVarText;
    }

    QString longestVarLabelText() {
        if (mLongestVarLabelText.isEmpty()) {
            for (auto iter=InitialLabelFilter.LabelCheckStates[Qt::Horizontal].constKeyValueBegin();
                 iter!=InitialLabelFilter.LabelCheckStates[Qt::Horizontal].constKeyValueEnd(); ++iter) {
                if (iter->first.size() > mLongestVarLabelText.size())
                    mLongestVarLabelText = iter->first;
            }
        }
        return mLongestVarLabelText;
    }

    const SymbolInfo& sectionSymbol(int sectionIndex, Qt::Orientation orientation)
    {
        if (orientation == Qt::Horizontal)
            return hSectionIndexToSymbol[sectionIndex];
        return vSectionIndexToSymbol[sectionIndex];
    }

public:
    int MaxEquationDimension = 0;
    int MaxVariableDimension = 0;

    LabelFilter  InitialLabelFilter;

private:
    ModelInstance* const mModelInstance;

    QVector<SymbolInfo> mEquations;
    QVector<SymbolInfo> mVariables;
    QMap<int, SymbolInfo> hSectionIndexToSymbol;
    QMap<int, SymbolInfo> vSectionIndexToSymbol;

    int mEquationEntries = 0;
    int mVariableEntries = 0;

    ValueFilter mInitialValueFilter;

    QString mLongestEqnText;
    QString mLongestEqnLabelText;

    QString mLongestVarText;
    QString mLongestVarLabelText;
};

ModelInstance::ModelInstance(const QString &workspace,
                             const QString &systemDir,
                             const QString &scratchDir)
    : mCache(new Cache(this))
    , mDataHandler(new DataHandler)
    , mScratchDir(scratchDir)
    , mWorkspace(QDir(workspace).absolutePath())
    , mSystemDir(systemDir)
{
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    if (mDCT) dctFree(&mDCT);
    delete mCache;
    delete mDataHandler;
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
    return mCache->sectionSymbol(sectionIndex, Qt::Vertical);
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
        if (sym.name() == name)
            return true;
    }
    return false;
}

QString ModelInstance::longestEqnText() const
{
    auto eqn = mCache->longestEqnText();
    auto label = mCache->longestEqnLabelText();
    if (eqn.size() > label.size())
        return eqn;
    return label;
}

QString ModelInstance::longestVarText() const
{
    auto var = mCache->longestVarText();
    auto label = mCache->longestVarLabelText();
    if (var.size() > label.size())
        return var;
    return label;
}

SymbolInfo ModelInstance::variable(int sectionIndex) const
{
    return mCache->sectionSymbol(sectionIndex, Qt::Horizontal);
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
    return mCache->MaxEquationDimension;
}

int ModelInstance::maxVariableDimension() const
{
    return mCache->MaxVariableDimension;
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
        if (sym.name() == name)
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
    loadInitialLabelFilter(Qt::Horizontal);
    loadInitialLabelFilter(Qt::Vertical);

    mDataHandler->loadDataMatrix(this);

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
            if (variant.toString().contains(INF, Qt::CaseInsensitive) ||
                    variant.toString().contains(EPS, Qt::CaseInsensitive))
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

    int rowidx = rowIndex(symbol.offset());
    QVector<MaxMin> scaling;
    if (rowidx >= 0 && !gmoGetRowSparse(mGMO, rowidx, colidx, jacval, nlflag, &nnz, &nlnnz)) {
        Q_FOREACH(const auto& var, symbols(dctvarSymType)) {
            MaxMin maxmin { false, gmoMinf(mGMO), gmoPinf(mGMO) };
            for (int i=0; i<nnz; ++i) {
                if (colidx[i] >= var.offset() && colidx[i] < var.lastOffset()) {
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
    int rowidx = rowIndex(symbol.offset());
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
        for (int i=symbol.offset(); i<symbol.offset()+symbol.entries(); ++i) {
            maxmin.first = std::max(maxmin.first, gmoGetRhsOne(mGMO, i));
            maxmin.second = std::min(maxmin.second, gmoGetRhsOne(mGMO, i));
        }
    }
    return maxmin;
}

QString ModelInstance::aggregatedRhs(const SymbolInfo &symbol) const
{
    double min = gmoGetRhsOne(mGMO, symbol.offset());
    double max = gmoGetRhsOne(mGMO, symbol.offset());
    if (symbol.entries()) {
        for (int i=1; i<symbol.entries(); ++i) {
            min = std::min(min, gmoGetRhsOne(mGMO, symbol.offset()+i));
            max = std::max(max, gmoGetRhsOne(mGMO, symbol.offset()+i));
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

    info.setFirstSection(sectionIndex);
    info.setOffset(dctSymOffset(mDCT, index));
    info.setDimension(dctSymDim(mDCT, index));
    info.setEntries(dctSymEntries(mDCT, index));

    char symbolName[GMS_SSSIZE];
    if (dctSymName(mDCT, index, symbolName, GMS_SSSIZE))
        info.setName("ERROR");
    else
        info.setName(symbolName);

    info.setType(dctSymType(mDCT, index));
    loadDimensions(info);
    loadLabelTree(info);

    return info;
}

void ModelInstance::loadDimensions(SymbolInfo &symbol) const
{
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbol.entries(); ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbol.offset() + j) < 0)
            continue;

        int symIndex;
        if (symbol.type() == dctvarSymType) {
            if (dctColUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains))
                continue;
        } else {
            if (dctRowUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains))
                continue;
        }

        char quote;
        char labelName[GMS_SSSIZE];
        QStringList labels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, labelName, GMS_SSSIZE);
            labels << labelName;
        }
        symbol.setLabels(symbol.firstSection()+j, labels);
    }
}

void ModelInstance::loadLabelTree(SymbolInfo &symbol) const
{
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    LabelTreeItem *root = new LabelTreeItem;
    for (int j=0; j<symbol.entries(); ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbol.offset() + j) < 0)
            continue;

        int symIndex;
        if (symbol.type() == dctvarSymType) {
            if (dctColUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains))
                continue;
        } else {
            if (dctRowUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains))
                continue;
        }

        char quote;
        char labelName[GMS_SSSIZE];
        QStringList labels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, labelName, GMS_SSSIZE);
            labels << labelName;
        }

        appendSubItems(root, labels);
    }

    QList<LabelTreeItem*> items { root->childs() };
    while (!items.isEmpty()) {
        QList<LabelTreeItem*> nextLevel;
        for (int nodes=items.size(), gap=symbol.entries()/nodes, currentSection=symbol.firstSection();
             nodes>0; --nodes, currentSection+=gap) {
            auto item = items.takeFirst();
            item->setSectionIndex(currentSection);
            if (!item->hasChildren())
                item->setSections({currentSection});
            nextLevel.append(item->childs());
        }
        items = nextLevel;
    }

    symbol.setLabelTree(QSharedPointer<LabelTreeItem>(root));
}

void ModelInstance::appendSubItems(LabelTreeItem *parent, QStringList &labels) const
{
    if (labels.isEmpty())
        return;
    auto label = labels.takeFirst();
    bool found = false;
    for (int i=0; i<parent->size(); ++i) {
        auto child = parent->child(i);
        if (child->text() == label) {
            found = true;
            appendSubItems(child, labels);
            break;
        }
    }
    if (!found) {
        auto child = new LabelTreeItem(label, parent);
        parent->append(child);
        appendSubItems(child, labels);
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

void ModelInstance::loadInitialLabelFilter(Qt::Orientation orientation)
{
    LabelCheckStates filter;
    Q_FOREACH(const auto &symbolInfo, symbols(orientation == Qt::Horizontal ? dctvarSymType : dcteqnSymType)) {
        int nDomains;
        int domains[GLOBAL_MAX_INDEX_DIM];
        for (int j=0; j<symbolInfo.entries(); ++j) {
            if (gmoGetjSolverQuiet(mGMO, symbolInfo.offset() + j) < 0)
                continue;

            int symIndex;
            if (orientation == Qt::Horizontal) {
                if (dctColUels(mDCT, symbolInfo.offset()+j, &symIndex, domains, &nDomains))
                    continue;
            } else {
                if (dctRowUels(mDCT, symbolInfo.offset()+j, &symIndex, domains, &nDomains))
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
    mCache->InitialLabelFilter.LabelCheckStates[orientation] = filter;
}

QVariant ModelInstance::data(int row, int column) const
{
    return mDataHandler->data(row, column);
}

void ModelInstance::setAppliedAggregation(const Aggregation &aggregation)
{
    mDataHandler->setAppliedAggregation(aggregation);
    mDataHandler->aggregate(this);
}

int ModelInstance::headerData(int logicalIndex, Qt::Orientation orientation) const
{
    return mDataHandler->headerData(logicalIndex, orientation);
}

QString ModelInstance::headerData(int sectionIndex, int dimension, Qt::Orientation orientation) const
{
    if (sectionIndex < PredefinedHeaderLength)
        return PredefinedHeader.at(sectionIndex);
    if (orientation == Qt::Horizontal) {
        auto var = variable(sectionIndex);
        if (dimension < 0)
            return var.name();
        return var.label(sectionIndex, dimension);
    }
    auto eqn = equation(sectionIndex);
    if (dimension < 0)
        return eqn.name();
    return eqn.label(sectionIndex, dimension);
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
    if (compare(sym.name())) {
        result.append(SearchResult{logicalIndex, orientation});
    } else {
        auto labels = sym.sectionLabels()[sectionIndex];
        Q_FOREACH(const auto label, labels) {
            if (compare(label)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

IdentifierStates ModelInstance::initialSymbolFilter(QAbstractItemModel *model,
                                                    Qt::Orientation orientation)
{
    int sections = orientation == Qt::Horizontal ? model->columnCount() :
                                                   model->rowCount();
    bool ok;
    IdentifierStates states;
    QSet<QString> symNames;
    for (int s=0; s<sections; ++s) {
        int realSection = model->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        auto data = headerData(realSection, -1, orientation);
        if (realSection < PredefinedHeaderLength) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        } else if (!symNames.contains(data)) {
            symNames.insert(data);
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        }
    }
    return states;
}

LabelFilter ModelInstance::initialLabelFilter() const
{
    return mCache->InitialLabelFilter;
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

    mLogMessages << "GAMS System Dir: " + mSystemDir;

    char msg[GMS_SSSIZE];
    if (!gevCreateD(&mGEV,
                    mSystemDir.toStdString().c_str(),
                    msg,
                    sizeof(msg)))
        mLogMessages << "ERROR: " + QString(msg);

    gmoSetExitIndicator(0); // switch of lib exit() call
    gmoSetScreenIndicator(0); // switch off std lib output
    gmoSetErrorCallback(ModelInstance::errorCallback);

    if (!gmoCreateD(&mGMO,
                    mSystemDir.toStdString().c_str(),
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
                    mSystemDir.toStdString().c_str(),
                    msg,
                    sizeof(msg))) {
        mLogMessages << "ERROR: " + QString(msg);
    }

    mInitialized = true;
}

DataRow ModelInstance::jaccobianRow(int row)
{
    const int columns = variableCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    DataRow jacRow;
    if (!gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz)) {
        for (int i=0; i<nz; ++i) {
            jacRow[colidx[i]] = jacval[i];
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
    if (!header.compare(Level, Qt::CaseInsensitive)) {
        value = gmoGetVarLOne(mGMO, column);
    } else if (!header.compare(Lower, Qt::CaseInsensitive)) {
        value = gmoGetVarLowerOne(mGMO, column);
    } else if (!header.compare(Marginal, Qt::CaseInsensitive)) {
        value = gmoGetVarMOne(mGMO, column);
        return specialMarginalVarValuePtr(value, column);
    } else if (!header.compare(Scale, Qt::CaseInsensitive)) {
        value = gmoGetVarScaleOne(mGMO, column);
    } else if (!header.compare(Upper, Qt::CaseInsensitive)) {
        value = gmoGetVarUpperOne(mGMO, column);
    }
    return specialValue(value);
}

QVariant ModelInstance::verticalAttribute(const QString &header, int row)
{
    double value = 0.0;
    if (!header.compare(Level, Qt::CaseInsensitive)) {
        value = gmoGetEquLOne(mGMO, row);
    } else if (!header.compare(Lower, Qt::CaseInsensitive)) {
        auto bounds = equationBounds(row);
        value = bounds.first;
    } else if (!header.compare(Marginal, Qt::CaseInsensitive)) {
        value = gmoGetEquMOne(mGMO, row);
        return specialMarginalEquValuePtr(value, row);
    } else if (!header.compare(Scale, Qt::CaseInsensitive)) {
        value = gmoGetEquScaleOne(mGMO, row);
    } else if (!header.compare(Upper, Qt::CaseInsensitive)) {
        auto bounds = equationBounds(row);
        value = bounds.second;
    }
    return specialValue(value);
}

bool ModelInstance::aggregationActive() const
{
    return mDataHandler->isAggregationActive();
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
        return P_INF;
    else if (gmoMinf(mGMO) == value)
        return N_INF;
    return value;
}

QVariant ModelInstance::specialMarginalValue(double value)
{
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalEquValueBasis(double value, int rIndex)
{
    if (gmoGetEquStatOne(mGMO, rIndex) != gmoBstat_Basic && value == 0.0)
        return EPS;
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalVarValueBasis(double value, int cIndex)
{
    if (gmoGetVarStatOne(mGMO, cIndex) != gmoBstat_Basic && value == 0.0)
        return EPS;
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
