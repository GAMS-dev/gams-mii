#ifndef ABSTRACTMODELINSTANCE_H
#define ABSTRACTMODELINSTANCE_H

#include "symbol.h"

#include <QString>
#include <QStringList>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

typedef QPair<double, double> Range;

class AbstractViewConfiguration;
class DataMatrix;

class AbstractModelInstance
{
protected:
    AbstractModelInstance(const QString &workspace,
                          const QString &systemDir,
                          const QString &scratchDir);

public:
    virtual ~AbstractModelInstance();

    QString workspace() const;

    void setWorkspace(const QString &workspace);

    QString systemDirectory() const;

    void setSystemDirectory(const QString &systemDir);

    QString scratchDirectory() const;

    void setScratchDirectory(const QString &scratchDir);

    bool useOutput() const;

    void setUseOutput(bool useOutput);

    double jaccMinimum() const;
    double jaccMaximum() const;

    double modelMinimum(ViewDataType type) const;
    void setModelMinimum(double value, ViewDataType type);

    double modelMaximum(ViewDataType type) const;
    void setModelMaximum(double value, ViewDataType type);

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
    virtual Symbol* equation(int sectionIndex) const;

    /**
     * @brief All equations known by the model instance.
     * @return All known equations.
     */
    virtual const QVector<Symbol*>& equations() const = 0;

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
    virtual Symbol* variable(int sectionIndex) const;

    /**
     * @brief All variables known by the model instance.
     * @return All known variables.
     */
    virtual const QVector<Symbol*>& variables() const = 0;

    virtual int coefficents() const = 0;

    virtual int positiveCoefficents() const = 0;

    virtual int negativeCoefficents() const = 0;

    virtual int nonLinearCoefficents() const = 0;

    virtual Range matrixRange() const = 0;

    virtual Range objectiveRange() const = 0;

    virtual Range boundsRange() const = 0;

    virtual Range rhsRange() const = 0;

    const QStringList& labels() const;

    virtual QString longestEquationText() const = 0;

    virtual QString longestVariableText() const = 0;

    virtual int maximumEquationDimension() const = 0;

    virtual int maximumVariableDimension() const = 0;

    virtual const QVector<Symbol*>& symbols(Symbol::Type type) const = 0;

    virtual void loadData() = 0;

    virtual int rowCount(int view) const = 0;

    virtual int rowCount(ViewDataType viewType) const = 0;

    virtual int rowEntries(int row, int view) const = 0;

    virtual int columnCount(int view) const = 0;

    virtual int columnCount(ViewDataType viewType) const = 0;

    virtual int columnEntries(int column, int view) const = 0;

    virtual QSharedPointer<AbstractViewConfiguration> clone(int view, int newView) = 0;

    virtual QVariant data(int row, int column, int view) const = 0;

    virtual QVariant data(int row, int column) const = 0;

    virtual QString headerData(int sectionIndex,
                               int dimension,
                               Qt::Orientation orientation) const = 0;

    virtual void aggregate(QSharedPointer<AbstractViewConfiguration> viewConfig) = 0;

    virtual int headerData(int logicalIndex,
                           Qt::Orientation orientation,
                           int view) const = 0;

    virtual void jaccobianData(DataMatrix& dataMatrix) = 0;

protected:
    QString mScratchDir;
    QString mWorkspace;
    QString mSystemDir;

    bool mUseOutput = false;

    double mModelJaccMinimum = std::numeric_limits<double>::max();
    double mModelJaccMaximum = std::numeric_limits<double>::lowest();

    QStringList mLogMessages;

    QStringList mLabels;
};

class EmptyModelInstance : public AbstractModelInstance
{
public:
    EmptyModelInstance(const QString &workspace = ".",
                       const QString &systemDir = QString(),
                       const QString &scratchDir = QString());

    const QVector<Symbol*>& equations() const override;

    const QVector<Symbol*>& variables() const override;

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

    const QVector<Symbol*>& symbols(Symbol::Type type) const override;

    void loadData() override;

    int rowCount(int view) const override;

    int rowCount(ViewDataType viewType) const override;

    int rowEntries(int row, int view) const override;

    int columnCount(int view) const override;

    int columnCount(ViewDataType viewType) const override;

    int columnEntries(int column, int view) const override;

    QSharedPointer<AbstractViewConfiguration> clone(int view, int newView) override;

    QVariant data(int row, int column, int view) const override;

    QVariant data(int row, int column) const override;

    QString headerData(int sectionIndex,
                       int dimension,
                       Qt::Orientation orientation) const override;

    void aggregate(QSharedPointer<AbstractViewConfiguration> viewConfig) override;

    int headerData(int logicalIndex,
                   Qt::Orientation orientation,
                   int view) const override;

    void jaccobianData(DataMatrix& dataMatrix) override;

private:
    QVector<Symbol*> mSymbols;
};

}
}
}

#endif // ABSTRACTMODELINSTANCE_H
