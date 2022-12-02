#include "abstractmodelinstance.h"

#include <QDir>

namespace gams {
namespace studio {
namespace modelinspector {

AbstractModelInstance::AbstractModelInstance(const QString &workspace,
                                             const QString &systemDir,
                                             const QString &scratchDir)
    : mScratchDir(scratchDir)
    , mWorkspace(QDir(workspace).absolutePath())
    , mSystemDir(systemDir)
{

}

AbstractModelInstance::~AbstractModelInstance()
{

}

QString AbstractModelInstance::workspace() const
{
    return mWorkspace;
}

void AbstractModelInstance::setWorkspace(const QString &workspace)
{
    mWorkspace = QDir(workspace).absolutePath();
}

QString AbstractModelInstance::systemDirectory() const
{
    return mSystemDir;
}

void AbstractModelInstance::setSystemDirectory(const QString &systemDir)
{
    mSystemDir = systemDir;
}

QString AbstractModelInstance::scratchDirectory() const
{
    return mScratchDir;
}

void AbstractModelInstance::setScratchDirectory(const QString &scratchDir)
{
    mScratchDir = scratchDir;
}

bool AbstractModelInstance::useOutput() const
{
    return mUseOutput;
}

void AbstractModelInstance::setUseOutput(bool useOutput)
{
    mUseOutput = useOutput;
}

double AbstractModelInstance::modelMinimum(PredefinedViewEnum type) const
{
    switch (type) {
    case PredefinedViewEnum::EqnAttributes:
        return mModelAttributeMinimumV;
    case PredefinedViewEnum::VarAttributes:
        return mModelAttributeMinimumH;
    case PredefinedViewEnum::Jaccobian:
        return mModelJaccMinimum;
    case PredefinedViewEnum::MinMax:
        return mModelMinMaxMinimum;
    case PredefinedViewEnum::Full:
        return std::min(mModelAttributeMinimumH, std::min(mModelAttributeMinimumV, mModelJaccMinimum));
    default:
        return 0.0;
    }
}

void AbstractModelInstance::setModelMinimum(double value, PredefinedViewEnum type)
{
    switch (type) {
    case PredefinedViewEnum::EqnAttributes:
        mModelAttributeMinimumV = value;
        break;
    case PredefinedViewEnum::VarAttributes:
        mModelAttributeMinimumH = value;
        break;
    case PredefinedViewEnum::Jaccobian:
        mModelJaccMinimum = value;
        break;
    case PredefinedViewEnum::MinMax:
        mModelMinMaxMinimum = value;
        break;
    default:
        return;
    }
}

double AbstractModelInstance::modelMaximum(PredefinedViewEnum type) const
{
    switch (type) {
    case PredefinedViewEnum::EqnAttributes:
        return mModelAttributeMaximumV;
    case PredefinedViewEnum::VarAttributes:
        return mModelAttributeMaximumH;
    case PredefinedViewEnum::Jaccobian:
        return mModelJaccMaximum;
    case PredefinedViewEnum::MinMax:
        return mModelMinMaxMaximum;
    case PredefinedViewEnum::Full:
        return std::max(mModelAttributeMaximumH, std::max(mModelAttributeMaximumV, mModelJaccMaximum));
    default:
        return 0.0;
    }
}

void AbstractModelInstance::setModelMaximum(double value, PredefinedViewEnum type)
{
    switch (type) {
    case PredefinedViewEnum::EqnAttributes:
        mModelAttributeMaximumV = value;
        break;
    case PredefinedViewEnum::VarAttributes:
        mModelAttributeMaximumH = value;
        break;
    case PredefinedViewEnum::Jaccobian:
        mModelJaccMaximum = value;
        break;
    case PredefinedViewEnum::MinMax:
        mModelMinMaxMaximum = value;
        break;
    default:
        return;
    }
}

QString AbstractModelInstance::logMessages() {
    auto messages = mLogMessages.join("\n");
    mLogMessages.clear();
    return messages;
}

QString AbstractModelInstance::modelName() const
{
    return QString();
}

int AbstractModelInstance::equationCount() const
{
    return 0;
}

int AbstractModelInstance::equationCount(EquationType type) const
{
    Q_UNUSED(type);
    return 0;
}

int AbstractModelInstance::equationRowCount() const
{
    return 0;
}

Symbol AbstractModelInstance::equation(int sectionIndex) const
{
    Q_UNUSED(sectionIndex);
    return Symbol();
}

int AbstractModelInstance::variableCount() const
{
    return 0;
}

int AbstractModelInstance::variableCount(VariableType type) const
{
    Q_UNUSED(type);
    return 0;
}

int AbstractModelInstance::variableRowCount() const
{
    return 0;
}

Symbol AbstractModelInstance::variable(int sectionIndex) const
{
    Q_UNUSED(sectionIndex);
    return Symbol();
}

EmptyModelInstance::EmptyModelInstance(const QString &workspace,
                                       const QString &systemDir,
                                       const QString &scratchDir)
    : AbstractModelInstance(workspace, systemDir, scratchDir)
{

}

void EmptyModelInstance::initialize()
{

}

const QVector<Symbol> &EmptyModelInstance::equations() const
{
    return mSymbols;
}

const QVector<Symbol> &EmptyModelInstance::variables() const
{
    return mSymbols;
}

int EmptyModelInstance::coefficents() const
{
    return 0;
}

int EmptyModelInstance::positiveCoefficents() const
{
    return 0;
}

int EmptyModelInstance::negativeCoefficents() const
{
    return 0;
}

int EmptyModelInstance::nonLinearCoefficents() const
{
    return 0;
}

Range EmptyModelInstance::matrixRange() const
{
    return Range();
}

Range EmptyModelInstance::objectiveRange() const
{
    return Range();
}

Range EmptyModelInstance::boundsRange() const
{
    return Range();
}

Range EmptyModelInstance::rhsRange() const
{
    return Range();
}

QString EmptyModelInstance::longestEquationText() const
{
    return QString();
}

QString EmptyModelInstance::longestVariableText() const
{
    return QString();
}

int EmptyModelInstance::maximumEquationDimension() const
{
    return 0;
}

int EmptyModelInstance::maximumVariableDimension() const
{
    return 0;
}

const QVector<Symbol> &EmptyModelInstance::symbols(Symbol::Type type) const
{
    Q_UNUSED(type);
    return mSymbols;
}

void EmptyModelInstance::loadData(LabelFilter &labelFilter)
{
    Q_UNUSED(labelFilter);
}

int EmptyModelInstance::rowCount(PredefinedViewEnum viewType) const
{
    Q_UNUSED(viewType);
    return 0;
}

int EmptyModelInstance::columnCount(PredefinedViewEnum viewType) const
{
    Q_UNUSED(viewType);
    return 0;
}

QVariant EmptyModelInstance::data(int row, int column, int view) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(view);
    return QVariant();
}

QString EmptyModelInstance::headerData(int sectionIndex,
                                       int dimension,
                                       Qt::Orientation orientation) const
{
    Q_UNUSED(sectionIndex);
    Q_UNUSED(dimension);
    Q_UNUSED(orientation);
    return QString();
}

void EmptyModelInstance::aggregate(const Aggregation &aggregation)
{
    Q_UNUSED(aggregation);
}

int EmptyModelInstance::headerData(int logicalIndex,
                                   Qt::Orientation orientation,
                                   int view) const
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(orientation);
    Q_UNUSED(view);
    return 0;
}

void EmptyModelInstance::searchHeaderData(int logicalIndex,
                                          int sectionIndex,
                                          const QString &term,
                                          bool isRegEx,
                                          DataSource symbolType,
                                          Qt::Orientation orientation,
                                          QList<SearchResult> &result)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(sectionIndex);
    Q_UNUSED(term);
    Q_UNUSED(isRegEx);
    Q_UNUSED(symbolType);
    Q_UNUSED(orientation);
    Q_UNUSED(result);
}

}
}
}
