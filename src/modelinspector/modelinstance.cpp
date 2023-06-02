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

namespace gams {
namespace studio {
namespace modelinspector {

ModelInstance::ModelInstance(const QString &workspace,
                             const QString &systemDir,
                             const QString &scratchDir)
    : AbstractModelInstance(workspace, systemDir, scratchDir)
    , mDataHandler(new DataHandler(*this))
{
    initialize();
    loadScratchData();
}

ModelInstance::~ModelInstance()
{
    if (mGMO) gmoFree(&mGMO);
    if (mGEV) gevFree(&mGEV);
    // don't delete mDCT... it is handled in GMO
    delete mDataHandler;
    qDeleteAll(mEquations);
    qDeleteAll(mVariables);
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

Symbol* ModelInstance::equation(int sectionIndex) const
{
    if (sectionIndex >= vSectionIndexToSymbol.size())
        return nullptr;
    return vSectionIndexToSymbol[sectionIndex];
}

const QVector<Symbol*>& ModelInstance::equations() const
{
    return mEquations;
}

int ModelInstance::equationCount() const
{
    return mEquations.count();
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

char ModelInstance::equationType(int row) const
{
    char buffer[GMS_SSSIZE];
    gmoGetEquTypeTxt(mGMO, row, buffer);
    return QString(buffer).replace('=', "").trimmed().at(0).toLatin1();
}

int ModelInstance::equationRowCount() const
{
    return gmoM(mGMO);
}

int ModelInstance::variableCount() const
{
    return mVariables.count();
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

char ModelInstance::variableType(int column) const
{
    char buffer[GMS_SSSIZE];
    gmoGetVarTypeTxt(mGMO, column, buffer);
    return QString(buffer).at(0).toLatin1();
}

int ModelInstance::variableRowCount() const
{
    return gmoN(mGMO);
}

QString ModelInstance::longestEquationText() const
{
    return mLongestEqnText;
    //return mLongestEqnText.size() > mLongestLabel.size() ? mLongestEqnText : mLongestLabel;
}

QString ModelInstance::longestVariableText() const
{
    return mLongestVarText;
    //return mLongestVarText.size() > mLongestLabel.size() ? mLongestVarText : mLongestLabel;
}

Symbol* ModelInstance::variable(int sectionIndex) const
{
    if (sectionIndex >= hSectionIndexToSymbol.size())
        return nullptr;
    return hSectionIndexToSymbol[sectionIndex];
}

const QVector<Symbol*>& ModelInstance::variables() const
{
    return mVariables;
}

int ModelInstance::symbolCount() const {
    return dctNLSyms(mDCT);
}

int ModelInstance::maximumEquationDimension() const
{
    return mMaxEquationDimension;
}

int ModelInstance::maximumVariableDimension() const
{
    return mMaxVariableDimension;
}

void ModelInstance::loadScratchData()
{
    mLogMessages << "Model Workspace: " + mWorkspace;
    QString ctrlFile = mWorkspace + "/" + mScratchDir + "/gamscntr.dat";
    mLogMessages << "CTRL File: " + ctrlFile;
    if (gevInitEnvironmentLegacy(mGEV, ctrlFile.toStdString().c_str())) {
        mLogMessages << "ERROR: Could not initialize model instance";
        mState = Error;
        return;
    }

    char msg[GMS_SSSIZE];
    gmoRegisterEnvironment(mGMO, mGEV, msg);
    if (gmoLoadDataLegacy(mGMO, msg)) {
        mLogMessages << "ERROR: Could not load model instance (input): " + QString(msg);
        mState = Error;
        return;
    }

    if (mUseOutput) {
        QString solFile = mWorkspace + "/" + mScratchDir + "/gamssolu.dat";
        mLogMessages << "Solution File: " + solFile;
        gmoNameSolFileSet(mGMO, solFile.toStdString().c_str());
        if (gmoLoadSolutionLegacy(mGMO)) {
            mLogMessages << "ERROR: Could not load model instance (output): " + QString(msg);
            mState = Error;
            return;
        }
    }

    // TODO (AF/LW) specifc error message that model insp. needs the dct... because it
    //      can be switched off by the user

    mDCT = (dctHandle_t)gmoDict(mGMO);
    if (!mDCT) {
        mLogMessages << "ERROR: Could not load dictionary file.";
        mState = Error;
        return;
    }

    mLogMessages << "Absolute Scrach Path: " + mScratchDir;
}

void ModelInstance::loadSymbols()
{
    int eqnIndex = 0, varIndex = 0;
    int sectionIndexEqn = 0, sectionIndexVar = 0;
    for (int i=1; i<=symbolCount(); ++i) {
        auto sym = loadSymbol(i);
        if (Symbol::Equation == sym->type()) {
            mMaxEquationDimension = std::max(mMaxEquationDimension, sym->dimension());
            sym->setFirstSection(sectionIndexEqn);
            sym->setLogicalIndex(eqnIndex++);
            sectionIndexEqn += sym->entries();
            loadEquationDimensions(sym); // TODO !!! PERF optimize or load/lazy load
            sym->setLabelTree(QSharedPointer<LabelTreeItem>(new LabelTreeItem));
            mEquations.append(sym);
            for (int i=sym->firstSection(); i<=sym->lastSection(); ++i) {
                vSectionIndexToSymbol.append(sym);
            }
            if (sym->name().size() > mLongestEqnText.size()) {
                mLongestEqnText = sym->name().left(10); // TODO !!! fix header space issue
            }
        } else if (Symbol::Variable == sym->type()) {
            mMaxVariableDimension = std::max(mMaxVariableDimension, sym->dimension());
            sym->setFirstSection(sectionIndexVar);
            sym->setLogicalIndex(varIndex++);
            sectionIndexVar += sym->entries();
            loadVariableDimensions(sym); // TODO !!! PERF optimize or load/lazy load
            sym->setLabelTree(QSharedPointer<LabelTreeItem>(new LabelTreeItem));
            mVariables.append(sym);
            for (int i=sym->firstSection(); i<=sym->lastSection(); ++i) {
                hSectionIndexToSymbol.append(sym);
            }
            if (sym->name().size() > mLongestVarText.size()) {
                mLongestVarText = sym->name().left(10); // TODO !!! fix header space issue
            }
        } else {
            delete sym;
        }
    }
}

Symbol* ModelInstance::loadSymbol(int index)
{
    auto sym = new Symbol;
    if (index > symbolCount())
        return sym;

    sym->setOffset(dctSymOffset(mDCT, index));
    sym->setDimension(dctSymDim(mDCT, index));
    sym->setEntries(dctSymEntries(mDCT, index));

    char symbolName[GMS_SSSIZE];
    if (dctSymName(mDCT, index, symbolName, GMS_SSSIZE)) {
        sym->setName("##ERROR##");
    } else {
        sym->setName(symbolName);
    }

    auto type = dctSymType(mDCT, index);
    if (type == dcteqnSymType) {
        sym->setType(Symbol::Equation);
    } else if (type == dctvarSymType) {
        sym->setType(Symbol::Variable);
    } else {
        mLogMessages << QString("ERROR: loadSymbol() >> Unknown symbol type (%1) found in ModelInstance::loadSymbol()").arg(sym->name());
        sym->setType(Symbol::Unknown);
        return sym;
    }

    int count = 0;
    gdxStrIndex_t gdxIndex;
    gdxStrIndexPtrs_t gdxIndexPtr;
    GDXSTRINDEXPTRS_INIT(gdxIndex, gdxIndexPtr);
    if (dctSymDomNames(mDCT, index, gdxIndexPtr, &count)) {
        mLogMessages << QString("ERROR: Could not load symbol (%1) domains.").arg(sym->name());
    } else {
        for (int i=0; i<count; ++i) {
            sym->appendDomainLabel(gdxIndexPtr[i]);
        }
    }

    return sym;
}

void ModelInstance::loadEquationDimensions(Symbol *symbol)
{
    char quote;
    int nDomains = 0;
    char labelName[GMS_SSSIZE];
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbol->entries(); ++j) {
        if (gmoGetiSolverQuiet(mGMO, symbol->offset() + j) < 0) {
            mLogMessages << "ERROR: calling gmoGetiSolverQuiet() in ModelInstance::loadDimensions()";
            continue;
        }

        int symIndex;
        if (dctRowUels(mDCT, symbol->offset()+j, &symIndex, domains, &nDomains)) {
            mLogMessages << "ERROR: calling dctRowUels() in ModelInstance::loadDimensions()";
            continue;
        }

        QStringList labels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, labelName, GMS_SSSIZE);
            labels << labelName;
        }
        symbol->setLabels(symbol->firstSection()+j, labels);
    }
}

void ModelInstance::loadVariableDimensions(Symbol *symbol)
{
    char quote;
    int nDomains = 0;
    char labelName[GMS_SSSIZE];
    int domains[GLOBAL_MAX_INDEX_DIM];
    for (int j=0; j<symbol->entries(); ++j) {
        if (gmoGetjSolverQuiet(mGMO, symbol->offset() + j) < 0) {
            mLogMessages << "ERROR: calling gmoGetjSolverQuiet() in ModelInstance::loadDimensions()";
            continue;
        }

        int symIndex;
        if (dctColUels(mDCT, symbol->offset()+j, &symIndex, domains, &nDomains)) {
            mLogMessages << "ERROR: calling dctColUels() in ModelInstance::loadDimensions()";
            continue;
        }

        QStringList labels;
        for (int k=0; k<nDomains; ++k) {
            dctUelLabel(mDCT, domains[k], &quote, labelName, GMS_SSSIZE);
            labels << labelName;
        }
        symbol->setLabels(symbol->firstSection()+j, labels);
    }
}

const QVector<Symbol*>& ModelInstance::symbols(Symbol::Type type) const
{
    return type == Symbol::Equation ? mEquations : mVariables;
}

void ModelInstance::loadData()
{
    loadSymbols();
    loadLabels();
    mDataHandler->loadJaccobian();
}

void ModelInstance::variableLowerBounds(double *bounds)
{
    if (gmoGetVarLower(mGMO, bounds)) {
        mLogMessages << "variableLowerBounds() -> Something went wrong!";
    }
}

void ModelInstance::variableUpperBounds(double *bounds)
{
    if (gmoGetVarUpper(mGMO, bounds)) {
        mLogMessages << "variableUpperBounds() -> Something went wrong!";
    }
}

double ModelInstance::rhs(int row) const
{
    return gmoGetRhsOne(mGMO, row);
}

int ModelInstance::rowCount(int view) const
{
    return mDataHandler->rowCount(view);
}

int ModelInstance::rowEntries(int row, int view) const
{
    return mDataHandler->rowEntries(row, view);
}

int ModelInstance::columnCount(int view) const
{
    return mDataHandler->columnCount(view);
}

int ModelInstance::columnEntries(int column, int view) const
{
    return mDataHandler->columnEntries(column, view);
}

QSharedPointer<AbstractViewConfiguration> ModelInstance::clone(int view, int newView)
{
    return mDataHandler->clone(view, newView);
}

void ModelInstance::loadData(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    return mDataHandler->loadData(viewConfig);
}

void ModelInstance::loadLabels()
{// TODO (LW/AF) A dct call only giving real labels would be nice... to improve the efficency
    char q;
    char label[GMS_SSSIZE];
    for (int i=1; i<=dctNUels(mDCT); ++i) {
        dctUelLabel(mDCT, i, &q, label, GMS_SSSIZE);
        mLabels << label;
    }
    const QString ttlblk = "ttlblk";
    const QString mincolcnt = "mincolcnt";
    const QString minrowcnt = "minrowcnt";
    for (int i=0; i<3 && !mLabels.isEmpty(); ++i) {
        auto label = mLabels.last();
        if (label == ttlblk || label == mincolcnt || label == minrowcnt)
            mLabels.removeLast();
    }
    for (const auto& label : qAsConst(mLabels)) {
        if (label.size() > mLongestLabel.size())
            mLongestLabel = label.left(10); // TODO !!! fix header space issue
    }
}

QVariant ModelInstance::data(int row, int column, int view) const
{
    return mDataHandler->data(row, column, view);
}

QVariant ModelInstance::headerData(int logicalIndex,
                                   Qt::Orientation orientation,
                                   int view, int role) const
{
    if (role == Mi::IndexDataRole) {
        return mDataHandler->headerData(logicalIndex, orientation, view);
    }
    if (role == Mi::LabelDataRole) {
        return mDataHandler->plainHeaderData(orientation, view, logicalIndex, 0);
    }
    return QVariant();
}

QVariant ModelInstance::plainHeaderData(Qt::Orientation orientation,
                                        int view,
                                        int logicalIndex,
                                        int dimension) const
{
    return mDataHandler->plainHeaderData(orientation, view, logicalIndex, dimension);
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
        mState = Error;
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
        mState = Error;
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
        mState = Error;
        return;
    }
}

void ModelInstance::jaccobianData(DataMatrix& dataMatrix)
{
    int nz, unused1, unused2;
    int *nlflag = new int[variableRowCount()];
    for (int row=0; row<equationRowCount(); ++row) {
        if (gmoGetRowStat(mGMO, row, &nz, &unused1, &unused2))
            continue;
        auto* dataRow = dataMatrix.row(row);
        dataRow->setEntries(nz);
        dataRow->setColIdx(new int[nz]);
        dataRow->setData(new double[nz]);
        if (gmoGetRowSparse(mGMO, row, dataRow->colIdx(), dataRow->data(), nlflag, &unused1, &unused2))
            continue;
    }
    delete [] nlflag;
}

//double ModelInstance::horizontalAttribute(const QString &header, int column)
//{// TODO !!!
//    double value = 0.0;
//    if (!header.compare(constant->Level, Qt::CaseInsensitive)) {
//        value = gmoGetVarLOne(mGMO, column);
//    } else if (!header.compare(constant->Lower, Qt::CaseInsensitive)) {
//        value = gmoGetVarLowerOne(mGMO, column);
//    } else if (!header.compare(constant->Marginal, Qt::CaseInsensitive)) {
//        value = gmoGetVarMOne(mGMO, column);
//        //return specialMarginalVarValuePtr(value, column);
//    } else if (!header.compare(constant->Scale, Qt::CaseInsensitive)) {
//        value = gmoGetVarScaleOne(mGMO, column);
//    } else if (!header.compare(constant->Upper, Qt::CaseInsensitive)) {
//        value = gmoGetVarUpperOne(mGMO, column);
//    }
//    mModelAttributeMinimumH = ValueFilter::minValue(mModelAttributeMinimumH, value);
//    mModelAttributeMaximumH = ValueFilter::maxValue(mModelAttributeMaximumH, value);
//    return value;
//    //return specialValueMinMax(value, Qt::Horizontal);
//}
//
//double ModelInstance::verticalAttribute(const QString &header, int row)
//{// TODO !!!
//    double value = 0.0;
//    if (!header.compare(constant->Level, Qt::CaseInsensitive)) {
//        value = gmoGetEquLOne(mGMO, row);
//    } else if (!header.compare(constant->Lower, Qt::CaseInsensitive)) {
//        auto bounds = equationBounds(row);
//        value = bounds.first;
//    } else if (!header.compare(constant->Marginal, Qt::CaseInsensitive)) {
//        value = gmoGetEquMOne(mGMO, row);
//        //return specialMarginalEquValuePtr(value, row);
//    } else if (!header.compare(constant->Scale, Qt::CaseInsensitive)) {
//        value = gmoGetEquScaleOne(mGMO, row);
//    } else if (!header.compare(constant->Upper, Qt::CaseInsensitive)) {
//        auto bounds = equationBounds(row);
//        value = bounds.second;
//    }
//    mModelAttributeMinimumV = ValueFilter::minValue(mModelAttributeMinimumV, value);
//    mModelAttributeMaximumV = ValueFilter::maxValue(mModelAttributeMaximumV, value);
//    return value;
//    //return specialValueMinMax(value, Qt::Vertical);
//}

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
        return constant->P_INF;
    else if (gmoMinf(mGMO) == value)
        return constant->N_INF;
    else if (GMS_SV_EPS == value)
        return constant->EPS;
    return value;
}

QVariant ModelInstance::specialValueMinMax(double value)
{
    if (value == 0.0)
        return QVariant();
    else if (gmoPinf(mGMO) == value)
        return constant->P_INF;
    else if (gmoMinf(mGMO) == value)
        return constant->N_INF;
    else if (GMS_SV_EPS == value)
        return constant->EPS;
    return value;
}

QVariant ModelInstance::specialMarginalValue(double value)
{
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalEquValueBasis(double value, int rIndex)
{
    if (gmoGetEquStatOne(mGMO, rIndex) != gmoBstat_Basic && value == 0.0)
        return constant->EPS;
    return specialValue(value);
}

QVariant ModelInstance::specialMarginalVarValueBasis(double value, int cIndex)
{
    if (gmoGetVarStatOne(mGMO, cIndex) != gmoBstat_Basic && value == 0.0)
        return constant->EPS;
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
