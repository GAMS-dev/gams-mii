#ifndef GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H
#define GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H

#include "modelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ModelStatistic
{ // TODO remove
public:
    ModelStatistic(ModelInstance *modelInstance)
        : mModelInstance(modelInstance)
    {}

    int coefficents() const {
        return mModelInstance->coefficents();
    }

    int positiveCoefficents() const {
        return mModelInstance->positiveCoefficents();
    }

    int negativeCoefficents() const {
        return mModelInstance->negativeCoefficents();
    }

    int nonLinearCoefficents() const {
        return mModelInstance->nonLinearCoefficents();
    }

    int equations() const {
        return mModelInstance->equations();
    }

    int numberEquations(int type) const {
        return mModelInstance->equations(type);
    }

    int variables() const {
        return mModelInstance->variables();
    }

    int equationBlocks() const {
        return mModelInstance->equationBlocks();
    }

    int variableBlocks() const {
        return mModelInstance->variableBlocks();
    }

    int numberVariables(int type) const {
        return mModelInstance->variables(type);
    }

    QPair<double, double> matrixRange() const {
        return mModelInstance->matrixRange();
    }

    QPair<double, double> objectiveRange() const {
        return mModelInstance->objectiveRange();
    }

    QPair<double, double> boundsRange() const {
        return mModelInstance->boundsRange();
    }

    QPair<double, double> rhsRange() const {
        return mModelInstance->rhsRange();
    }

private:
    ModelInstance *mModelInstance;
};

} // namespace modelinspector
} // namespace studio
} // namespace gams

#endif // GAMS_STUDIO_MODELINSPECTOR_MODELSTATISTIC_H
