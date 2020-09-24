#ifndef GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H
#define GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H

#include "modelinstance.h"

#include <QPair>

namespace gams {
namespace studio {
namespace modelinspector {

class ModelStatistic
{
public:
    ModelStatistic(ModelInstance *modelInstance)
        : mModelInstance(modelInstance)
    {}

    int coefficents() const {
        return gmoNZ(mModelInstance->gmo());
    }

    int positiveCoefficents() const;

    int negativeCoefficents() const;

    int nonLinearCoefficents() const {
        return gmoNLNZ(mModelInstance->gmo());
    }

    int equations() const {
        return gmoM(mModelInstance->gmo());
    }

    /**
     * @brief Number if equation of equType.
     * @param equType Value of gmoEquType.
     * @return Number of equations of specified type.
     */
    int numberEquations(int equType) const {
        return gmoGetEquTypeCnt(mModelInstance->gmo(), equType);
    }

    int variables() const {
        return gmoN(mModelInstance->gmo());
    }

    int equationBlocks() const;

    int variableBlocks() const;

    /**
     * @brief Number of variables or varType.
     * @param varType Value of gmoVarType.
     * @return Number of variables of specified type.
     */
    int numberVariables(int varType) const {
        return gmoGetVarTypeCnt(mModelInstance->gmo(), varType);
    }

    QPair<double, double> matrixRange() const;

    QPair<double, double> objectiveRange() const;

    QPair<double, double> boundsRange() const;

    QPair<double, double> rhsRange() const;

private:
    ModelInstance *mModelInstance;
};

} // namespace modelinspector
} // namespace studio
} // namespace gams

#endif // GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H
