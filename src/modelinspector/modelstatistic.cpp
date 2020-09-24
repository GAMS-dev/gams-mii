#include "modelstatistic.h"

#include <algorithm>
#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

// TODO (AF) get full jac right away... large model issue?
int ModelStatistic::positiveCoefficents() const
{// TODO (AF) optimize... save value
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int positiveCoeffs = 0;
    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mModelInstance->gmo(), row, colidx, jacval, nlflag, &nz, &nlnz);
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

int ModelStatistic::negativeCoefficents() const
{// TODO (AF) optimize... save value
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;
    int negativeCoeffs = 0;
    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mModelInstance->gmo(), row, colidx, jacval, nlflag, &nz, &nlnz);
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

int ModelStatistic::equationBlocks() const
{
    int blocks = 0;
    for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
        auto symbol = mModelInstance->symbol(i);
        if (mModelInstance->isEquation(symbol.Type))
            ++blocks;
    }
    return blocks;
}

int ModelStatistic::variableBlocks() const
{
    int blocks = 0;
    for (int i=1; i<=mModelInstance->symbolCount(); ++i) {
        auto symbol = mModelInstance->symbol(i);
        if (mModelInstance->isVariable(symbol.Type))
            ++blocks;
    }
    return blocks;
}

QPair<double, double> ModelStatistic::matrixRange() const
{
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Fun);
    QPair<double, double> range;
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    // TODO filter zielfunktion?
    for (int row=0; row<equations(); ++row) {
        gmoGetRowSparse(mModelInstance->gmo(), row, colidx, jacval, nlflag, &nz, &nlnz);
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
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Var);
    return range;
}

QPair<double, double> ModelStatistic::objectiveRange() const
{
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Fun);
    QPair<double, double> range;
    const int columns = variables();
    int *colidx = new int[columns];
    double *jacval = new double[columns];
    int *nlflag = new int[columns];
    int nz;
    int nlnz;

    for (int row=0; row<equations(); ++row) {
        gmoGetObjSparse(mModelInstance->gmo(), colidx, jacval, nlflag, &nz, &nlnz);
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
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Var);
    return range;
}

QPair<double, double> ModelStatistic::boundsRange() const
{
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Fun);
    QPair<double, double> range;
    auto columns = variables();

    auto lowerVals = new double[columns];
    if (!gmoGetVarLower(mModelInstance->gmo(), lowerVals)) {
        qDebug() << "gmoGetVarLower";
        for (int i=0; i< columns; ++i) {
            if (i == 0) {
                range.first = lowerVals[i];
            } else {
                range.first = std::min(range.first, lowerVals[i]);
            }
        }
    }

    auto upperVals = new double[columns];
    if (!gmoGetVarUpper(mModelInstance->gmo(), upperVals)) {
        qDebug() << "gmoGetVarUpper";
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
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Var);
    return range;
}

QPair<double, double> ModelStatistic::rhsRange() const
{
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Fun);
    QPair<double, double> range;
    auto rows = equations();
    auto *vals = new double[rows];
    if (gmoGetRhs(mModelInstance->gmo(), vals))
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
    gmoObjStyleSet(mModelInstance->gmo(), gmoObjType_Var);
    return range;
}

} // namespace modelinspector
} // namespace studio
} // namespace gams
