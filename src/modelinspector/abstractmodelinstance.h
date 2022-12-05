#ifndef ABSTRACTMODELINSTANCE_H
#define ABSTRACTMODELINSTANCE_H

#include "symbol.h"
#include "aggregation.h"

#include <QString>
#include <QStringList>

namespace gams {
namespace studio {
namespace modelinspector {

typedef QPair<double, double> Range;

class AbstractModelInstance
{
protected:
    AbstractModelInstance(const QString &workspace,
                          const QString &systemDir,
                          const QString &scratchDir);

public:
    virtual ~AbstractModelInstance();

    virtual void initialize() = 0;

    QString workspace() const;

    void setWorkspace(const QString &workspace);

    QString systemDirectory() const;

    void setSystemDirectory(const QString &systemDir);

    QString scratchDirectory() const;

    void setScratchDirectory(const QString &scratchDir);

    bool useOutput() const;

    void setUseOutput(bool useOutput);

    double modelMinimum(PredefinedViewEnum type) const;
    void setModelMinimum(double value, PredefinedViewEnum type);

    double modelMaximum(PredefinedViewEnum type) const;
    void setModelMaximum(double value, PredefinedViewEnum type);

    QString logMessages();

    virtual QString modelName() const;

    /**
     * @brief Total number of equations.
     * @return Equation count.
     */
    virtual int equationCount() const;

    /**
     * @brief Number of equations defined by <c>type</c>.
     * @param GMO equation type.
     */
    virtual int equationCount(EquationType type) const;

    /**
     * @brief Total number of equation rows.
     * @return Equation row count.
     */
    virtual int equationRowCount() const;

    /**
     * @brief Specific equation by <c>sectionIndex</c>
     * @param Section index of equation in raw data matrix.
     * @return Equation.
     */
    virtual Symbol equation(int sectionIndex) const;

    /**
     * @brief All equations known by the model instance.
     * @return All known equations.
     */
    virtual const QVector<Symbol>& equations() const = 0;

    /**
     * @brief Total number of variables.
     */
    virtual int variableCount() const;

    /**
     * @brief Number of variables defined by <c>type</c>.
     * @param Equation type.
     */
    virtual int variableCount(VariableType type) const;

    /**
     * @brief Total number of variable rows.
     * @return Variable row count.
     */
    virtual int variableRowCount() const;

    /**
     * @brief Specific variable by <c>sectionIndex</c>
     * @param Section index of variable in raw data matrix.
     * @return Variable.
     */
    virtual Symbol variable(int sectionIndex) const;

    /**
     * @brief All variables known by the model instance.
     * @return All known variables.
     */
    virtual const QVector<Symbol>& variables() const = 0;

    virtual int coefficents() const = 0;

    virtual int positiveCoefficents() const = 0;

    virtual int negativeCoefficents() const = 0;

    virtual int nonLinearCoefficents() const = 0;

    virtual Range matrixRange() const = 0;

    virtual Range objectiveRange() const = 0;

    virtual Range boundsRange() const = 0;

    virtual Range rhsRange() const = 0;

    virtual QString longestEquationText() const = 0;

    virtual QString longestVariableText() const = 0;

    virtual int maximumEquationDimension() const = 0;

    virtual int maximumVariableDimension() const = 0;

    virtual const QVector<Symbol>& symbols(Symbol::Type type) const = 0;

    virtual void loadData(LabelFilter &labelFilter) = 0;

    virtual int rowCount(PredefinedViewEnum viewType) const = 0;

    virtual int columnCount(PredefinedViewEnum viewType) const = 0;

    virtual QVariant data(int row, int column, int view) const = 0;

    virtual QString headerData(int sectionIndex,
                               int dimension,
                               Qt::Orientation orientation) const = 0;

    virtual void aggregate(const Aggregation &aggregation) = 0;

    virtual int headerData(int logicalIndex,
                           Qt::Orientation orientation,
                           int view) const = 0;

protected:
    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;

    bool mUseOutput = false;

    double mModelAttributeMinimumH = 0.0;
    double mModelAttributeMaximumH = 0.0;

    double mModelAttributeMinimumV = 0.0;
    double mModelAttributeMaximumV = 0.0;

    double mModelJaccMinimum = 0.0;
    double mModelJaccMaximum = 0.0;

    double mModelMinMaxMinimum = 0.0;
    double mModelMinMaxMaximum = 0.0;

    QStringList mLogMessages;
};

class EmptyModelInstance : public AbstractModelInstance
{
public:
    EmptyModelInstance(const QString &workspace = ".",
                       const QString &systemDir = QString(),
                       const QString &scratchDir = QString());

    void initialize() override;

    const QVector<Symbol>& equations() const override;

    const QVector<Symbol>& variables() const override;

    int coefficents() const override;

    int positiveCoefficents() const override;

    int negativeCoefficents() const override;

    int nonLinearCoefficents() const override;

    Range matrixRange() const override;

    Range objectiveRange() const override;

    Range boundsRange() const override;

    Range rhsRange() const override;

    QString longestEquationText() const override;

    QString longestVariableText() const override;

    int maximumEquationDimension() const override;

    int maximumVariableDimension() const override;

    const QVector<Symbol>& symbols(Symbol::Type type) const override;

    void loadData(LabelFilter &labelFilter) override;

    int rowCount(PredefinedViewEnum viewType) const override;

    int columnCount(PredefinedViewEnum viewType) const override;

    QVariant data(int row, int column, int view) const override;

    QString headerData(int sectionIndex,
                       int dimension,
                       Qt::Orientation orientation) const override;

    void aggregate(const Aggregation &aggregation) override;

    int headerData(int logicalIndex,
                   Qt::Orientation orientation,
                   int view) const override;

private:
    QVector<Symbol> mSymbols;
};

}
}
}

#endif // ABSTRACTMODELINSTANCE_H
