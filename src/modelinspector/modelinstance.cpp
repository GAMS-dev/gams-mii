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
#include "labeltreeitem.h"

#include <QAbstractItemModel>
#include <QVector>

#include <QDebug>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

class ModelInstance::SymbolCache
{
public:
    SymbolCache(ModelInstance* const modelInstance)
        : mModelInstance(modelInstance)
    {

    }

    void loadSymbols(Symbol::Type type)
    {
        int sectionIndex = PredefinedHeaderLength;
        QVector<Symbol> &syms = (type == Symbol::Equation) ? mEquations : mVariables;
        for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
            auto sym = mModelInstance->loadSymbol(i, sectionIndex);
            if (type != sym.type())
                continue;
            sectionIndex += sym.entries();
            syms.append(sym);
        }
    }

    void loadHorizontalHeaderData()
    {
        int itemIndex = PredefinedHeaderLength;
        Q_FOREACH(const auto &var, symbols(Symbol::Variable)) {
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
        Q_FOREACH(const auto &eqn, symbols(Symbol::Equation)) {
            MaxEquationDimension = qMax(MaxEquationDimension, eqn.dimension());
            for (int i=itemIndex; i<itemIndex+eqn.entries(); ++i) {
                vSectionIndexToSymbol[i] = eqn;
            }
            itemIndex += eqn.entries();
        }
    }

    const QVector<Symbol>& symbols(Symbol::Type type) const
    {
        return type == Symbol::Equation ? mEquations : mVariables;
    }

    QString longestEqnText()
    {
        if (mLongestEqnText.isEmpty()) {
            Q_FOREACH(auto symbol, mModelInstance->equations()) {
                if (symbol.name().size() > mLongestEqnText.size())
                    mLongestEqnText = symbol.name();
            }
        }
        return mLongestEqnText;
    }

    QString longestEqnLabelText() const
    {
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

    QString longestVarLabelText() const
    {
        return mLongestVarLabelText;
    }

    void setLongestLabelText(Qt::Orientation orientation, const LabelFilter &labelFilter)
    {
        for (auto iter=labelFilter.LabelCheckStates[orientation].constKeyValueBegin();
             iter!=labelFilter.LabelCheckStates[orientation].constKeyValueEnd(); ++iter) {
            switch (orientation) {
            case Qt::Horizontal:
                if (iter->first.size() > mLongestVarLabelText.size()) {
                    mLongestVarLabelText = iter->first;
                }
                break;
            default:
                if (iter->first.size() > mLongestEqnLabelText.size()) {
                    mLongestEqnLabelText = iter->first;
                }
                break;
            }
        }
    }

    const Symbol& sectionSymbol(int sectionIndex, Qt::Orientation orientation)
    {
        if (orientation == Qt::Horizontal)
            return hSectionIndexToSymbol[sectionIndex];
        return vSectionIndexToSymbol[sectionIndex];
    }

public:
    int MaxEquationDimension = 0;
    int MaxVariableDimension = 0;

private:
    ModelInstance* const mModelInstance;

    QVector<Symbol> mEquations;
    QVector<Symbol> mVariables;
    QMap<int, Symbol> hSectionIndexToSymbol;
    QMap<int, Symbol> vSectionIndexToSymbol;

    QString mLongestEqnText;
    QString mLongestEqnLabelText;

    QString mLongestVarText;
    QString mLongestVarLabelText;
};

ModelInstance::ModelInstance(const QString &workspace,
                             const QString &systemDir,
                             const QString &scratchDir)
    : AbstractModelInstance(workspace, systemDir, scratchDir)
    , mSymbolCache(new SymbolCache(this))
    , mDataHandler(new DataHandler)
{
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    // don't delete mDCT... it is handled in GMO
    delete mSymbolCache;
    delete mDataHandler;
}

QString ModelInstance::modelName() const
{
    if (mGMO) {
        char name[GMS_SSSIZE];
        gmoNameModel(mGMO, name);
        return name;
    }
    return QString();
}

int ModelInstance::coefficents() const
{
    return gmoNZ(mGMO);
}

int ModelInstance::positiveCoefficents() const
{
    const int columns = variableRowCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int positiveCoeffs = 0;
    for (int row=0; row<equationRowCount(); ++row) {
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
    const int columns = variableRowCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int negativeCoeffs = 0;
    for (int row=0; row<equationRowCount(); ++row) {
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

Symbol ModelInstance::equation(int sectionIndex) const
{
    return mSymbolCache->sectionSymbol(sectionIndex, Qt::Vertical);
}

const QVector<Symbol>& ModelInstance::equations() const
{
    return mSymbolCache->symbols(Symbol::Equation);
}

int ModelInstance::equationCount() const
{
    return symbols(Symbol::Equation).count();
}

int ModelInstance::equationCount(EquationType type) const
{
    switch (type) {
    case EquationType::E:
        return gmoGetEquTypeCnt(mGMO, gmoequ_E);
    case EquationType::G:
        return gmoGetEquTypeCnt(mGMO, gmoequ_G);
    case EquationType::L:
        return gmoGetEquTypeCnt(mGMO, gmoequ_L);
    case EquationType::N:
        return gmoGetEquTypeCnt(mGMO, gmoequ_N);
    case EquationType::X:
        return gmoGetEquTypeCnt(mGMO, gmoequ_X);
    case EquationType::C:
        return gmoGetEquTypeCnt(mGMO, gmoequ_C);
    case EquationType::B:
        return gmoGetEquTypeCnt(mGMO, gmoequ_B);
    default:
        return 0;
    }
}

int ModelInstance::equationRowCount() const
{
    return gmoM(mGMO);
}

int ModelInstance::variableCount() const
{
    return symbols(Symbol::Variable).count();
}

int ModelInstance::variableCount(VariableType type) const
{
    switch (type) {
    case VariableType::X:
        return gmoGetVarTypeCnt(mGMO, gmovar_X);
    case VariableType::B:
        return gmoGetVarTypeCnt(mGMO, gmovar_B);
    case VariableType::I:
        return gmoGetVarTypeCnt(mGMO, gmovar_I);
    case VariableType::S1:
        return gmoGetVarTypeCnt(mGMO, gmovar_S1);
    case VariableType::S2:
        return gmoGetVarTypeCnt(mGMO, gmovar_S2);
    case VariableType::SC:
        return gmoGetVarTypeCnt(mGMO, gmovar_SC);
    case VariableType::SI:
        return gmoGetVarTypeCnt(mGMO, gmovar_SI);
    default:
        return 0;
    }
}

int ModelInstance::variableRowCount() const
{
    return gmoN(mGMO);
}

QString ModelInstance::longestEquationText() const
{
    auto eqn = mSymbolCache->longestEqnText();
    auto label = mSymbolCache->longestEqnLabelText();
    if (eqn.size() > label.size())
        return eqn;
    return label;
}

QString ModelInstance::longestVariableText() const
{
    auto var = mSymbolCache->longestVarText();
    auto label = mSymbolCache->longestVarLabelText();
    if (var.size() > label.size())
        return var;
    return label;
}

Symbol ModelInstance::variable(int sectionIndex) const
{
    return mSymbolCache->sectionSymbol(sectionIndex, Qt::Horizontal);
}

const QVector<Symbol>& ModelInstance::variables() const
{
    return mSymbolCache->symbols(Symbol::Variable);
}

int ModelInstance::symbolCount() const {
    return dctNLSyms(mDCT);
}

int ModelInstance::maximumEquationDimension() const
{
    return mSymbolCache->MaxEquationDimension;
}

int ModelInstance::maximumVariableDimension() const
{
    return mSymbolCache->MaxVariableDimension;
}

void ModelInstance::loadScratchData(bool useOutput)
{
    setUseOutput(useOutput);
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
        mLogMessages << "ERROR: Could not load model instance (input): " + QString(msg);
        return;
    }
    if (mUseOutput) {
        QString solFile = mWorkspace + "/" + mScratchDir + "/gamssolu.dat";
        mLogMessages << "Solution File: " + solFile;
        gmoNameSolFileSet(mGMO, solFile.toStdString().c_str());
        if (gmoLoadSolutionLegacy(mGMO)) {
            mLogMessages << "ERROR: Could not load model instance (output): " + QString(msg);
            return;
        }
    }

    // TODO (AF/LW) specifc error message that model insp. needs the dct... because it
    //      can be switched off by the user

    mDCT = (dctHandle_t)gmoDict(mGMO);
    if (!mDCT) {
        mLogMessages << "ERROR: Could not load dictionary file.";
        return;
    }

    mLogMessages << "Absolute Scrach Path: " + mScratchDir;
}

void ModelInstance::loadTableData(LabelFilter &labelFilter)
{
    mSymbolCache->loadSymbols(Symbol::Equation);
    mSymbolCache->loadSymbols(Symbol::Variable);
    loadInitialLabelFilter(Qt::Horizontal, labelFilter);
    loadInitialLabelFilter(Qt::Vertical, labelFilter);

    mDataHandler->loadDataMatrix(this);
    mSymbolCache->loadHorizontalHeaderData();
    mSymbolCache->loadVerticalHeaderData();
}

Symbol ModelInstance::loadSymbol(int index, int sectionIndex)
{
    Symbol info;
    if (index > symbolCount())
        return info;

    info.setFirstSection(sectionIndex);
    info.setOffset(dctSymOffset(mDCT, index));
    info.setDimension(dctSymDim(mDCT, index));
    info.setEntries(dctSymEntries(mDCT, index));

    char symbolName[GMS_SSSIZE];
    if (dctSymName(mDCT, index, symbolName, GMS_SSSIZE))
        info.setName("##ERROR##");
    else
        info.setName(symbolName);

    auto type = dctSymType(mDCT, index);
    if (type == dcteqnSymType) {
        info.setType(Symbol::Equation);
    } else if (type == dctvarSymType) {
        info.setType(Symbol::Variable);
    } else {
        mLogMessages << QString("ERROR: loadSymbol() >> Unknown symbol type (%1) found in ModelInstance::loadSymbol()").arg(info.name());
        info.setType(Symbol::Unknown);
        return info;
    }

    int count;
    gdxStrIndex_t gdxIndex;
    gdxStrIndexPtrs_t gdxIndexPtr;
    GDXSTRINDEXPTRS_INIT(gdxIndex, gdxIndexPtr);
    if (dctSymDomNames(mDCT, index, gdxIndexPtr, &count)) {
        mLogMessages << QString("ERROR: Could not load symbol (%1) domains.").arg(info.name());
    } else {
        for (int i=0; i<count; ++i) {
            info.appendDomainLabel(gdxIndexPtr[i]);
        }
    }

    loadDimensions(info);
    loadLabelTree(info);

    return info;
}

void ModelInstance::loadDimensions(Symbol &symbol)
{
    int nDomains = 0;
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbol.entries(); ++j) {
        if (symbol.isVariable() && gmoGetjSolverQuiet(mGMO, symbol.offset() + j) < 0) {
            mLogMessages << "ERROR: calling gmoGetjSolverQuiet() in ModelInstance::loadDimensions()";
            continue;
        } else if (symbol.isEquation() && gmoGetiSolverQuiet(mGMO, symbol.offset() + j) < 0) {
            mLogMessages << "ERROR: calling gmoGetiSolverQuiet() in ModelInstance::loadDimensions()";
            continue;
        }

        int symIndex;
        if (symbol.isVariable() && dctColUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains)) {
            mLogMessages << "ERROR: calling dctColUels() in ModelInstance::loadDimensions()";
            continue;
        } else if (symbol.isEquation() && dctRowUels(mDCT, symbol.offset()+j, &symIndex, domains, &nDomains)) {
            mLogMessages << "ERROR: calling dctRowUels() in ModelInstance::loadDimensions()";
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

void ModelInstance::loadLabelTree(Symbol &symbol) const
{
    int nDomains;
    int domains[GLOBAL_MAX_INDEX_DIM];
    LabelTreeItem *root = new LabelTreeItem;
    for (int j=0; j<symbol.entries(); ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbol.offset() + j) < 0)
            continue;

        int symIndex;
        if (symbol.isVariable()) {
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

const QVector<Symbol>& ModelInstance::symbols(Symbol::Type type) const
{
    return mSymbolCache->symbols(type);
}

void ModelInstance::loadData(bool useOutput, LabelFilter &labelFilter)
{
    loadScratchData(useOutput);
    loadTableData(labelFilter);
}

Range ModelInstance::matrixRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    Range range;
    const int columns = variableRowCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equationRowCount(); ++row) {
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

Range ModelInstance::objectiveRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    Range range;
    const int columns = variableRowCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equationRowCount(); ++row) {
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

Range ModelInstance::boundsRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    Range range;
    auto columns = variableRowCount();

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

Range ModelInstance::rhsRange() const
{
    gmoObjStyleSet(mGMO, gmoObjType_Fun);
    Range range;
    auto rows = equationRowCount();
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

int ModelInstance::rowCount(PredefinedViewEnum viewType) const
{
    switch (viewType) {
    case PredefinedViewEnum::VarAttributes:
        return PredefinedHeaderLength;
    case PredefinedViewEnum::MinMax: // one row for max and min
        return PredefinedHeaderLength + (equationCount() * 2);
    default:
        return PredefinedHeaderLength + equationRowCount();
    }
}

int ModelInstance::columnCount(PredefinedViewEnum viewType) const
{
    switch (viewType) {
    case PredefinedViewEnum::EqnAttributes:
        return PredefinedHeaderLength;
    case PredefinedViewEnum::MinMax:
        return PredefinedHeaderLength + variableCount();
    default:
        return PredefinedHeaderLength + variableRowCount();
    }
}

void ModelInstance::loadInitialLabelFilter(Qt::Orientation orientation, LabelFilter &labelFilter)
{
    LabelCheckStates filter;
    Q_FOREACH(const auto &symbolInfo, symbols(orientation == Qt::Horizontal ? Symbol::Variable : Symbol::Equation)) {
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
    labelFilter.LabelCheckStates[orientation] = filter;
    mSymbolCache->setLongestLabelText(orientation, labelFilter);
}

QVariant ModelInstance::data(int row, int column, int view) const
{
    return mDataHandler->data(row, column, view);
}

void ModelInstance::aggregate(const Aggregation &aggregation)
{
    mDataHandler->aggregate(aggregation, this);
}

int ModelInstance::headerData(int logicalIndex,
                              Qt::Orientation orientation,
                              int view) const
{
    return mDataHandler->headerData(logicalIndex, orientation, view);
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

void ModelInstance::searchHeaderData(int logicalIndex,
                                     int sectionIndex,
                                     const QString &term,
                                     bool isRegEx,
                                     DataSource dataSource,
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

    if (sectionIndex < PredefinedHeaderLength && compare(PredefinedHeader.at(sectionIndex))) {
        result.append(SearchResult{logicalIndex, orientation});
        return;
    }

    auto sourceOrientation = dataSource == DataSource::VariableData ? Qt::Horizontal
                                                                    : Qt::Vertical;
    auto sym = mSymbolCache->sectionSymbol(sectionIndex, sourceOrientation);
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
                    sizeof(msg))) {
        mLogMessages << "ERROR: " + QString(msg);
        return;
    }

    gmoSetExitIndicator(0); // switch of lib exit() call
    gmoSetScreenIndicator(0); // switch off std lib output
    gmoSetErrorCallback(ModelInstance::errorCallback);

    if (!gmoCreateD(&mGMO,
                    mSystemDir.toStdString().c_str(),
                    msg,
                    sizeof(msg))) {
        mLogMessages << "ERROR: " + QString(msg);
        return;
    }

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
        return;
    }
}

DataRow ModelInstance::jaccobianRow(int row)
{
    const int columns = variableRowCount();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    DataRow jacRow;
    if (!gmoGetRowSparse(mGMO, row, colidx, jacval, nlflag, &nz, &nlnz)) {
        for (int i=0; i<nz; ++i) {
            jacRow[colidx[i]] = jacval[i];
            mModelMinimum = std::min(mModelMinimum, jacval[i]);
            mModelMaximum = std::max(mModelMaximum, jacval[i]);
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
    return specialValueMinMax(value);
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
    return specialValueMinMax(value);
}

QPair<double, double> ModelInstance::equationBounds(int row)
{
    QPair<double, double> bounds;
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
    else if (GMS_SV_EPS == value)
        return EPS;
    return value;
}

QVariant ModelInstance::specialValueMinMax(double value)
{
    if (value == 0.0)
        return QVariant();
    else if (gmoPinf(mGMO) == value)
        return P_INF;
    else if (gmoMinf(mGMO) == value)
        return N_INF;
    else if (GMS_SV_EPS == value)
        return EPS;
    mModelMinimum = std::min(mModelMinimum, value);
    mModelMaximum = std::max(mModelMaximum, value);
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
