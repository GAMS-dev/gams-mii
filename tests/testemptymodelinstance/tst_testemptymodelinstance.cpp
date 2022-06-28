#include <QtTest>

#include "abstractmodelinstance.h"

using namespace gams::studio::modelinspector;

class TestEmptyModelInstance : public QObject
{
    Q_OBJECT

public:
    TestEmptyModelInstance();
    ~TestEmptyModelInstance();

private slots:
    void test_constructor_initialize();
    void test_default();
    void test_getSet();
};

TestEmptyModelInstance::TestEmptyModelInstance()
{

}

TestEmptyModelInstance::~TestEmptyModelInstance()
{

}

void TestEmptyModelInstance::test_constructor_initialize()
{
    const QString workspace = "myWorkDir";
    const QString systemDir = "mySysDir";
    const QString scratchDir = "myScratchDir";
    EmptyModelInstance instance(workspace, systemDir, scratchDir);
    auto realWorkspace = QDir(workspace).absolutePath();
    QCOMPARE(instance.workspace(), realWorkspace);
    QCOMPARE(instance.systemDirectory(), systemDir);
    QCOMPARE(instance.scratchDirectory(), scratchDir);
    instance.initialize();
}

void TestEmptyModelInstance::test_default()
{
    EmptyModelInstance instance;
    QCOMPARE(instance.workspace(), QDir(".").absolutePath());
    QCOMPARE(instance.systemDirectory(), QString());
    QCOMPARE(instance.scratchDirectory(), QString());
    QCOMPARE(instance.useOutput(), false);
    QCOMPARE(instance.modelMinimum(), 0.0);
    QCOMPARE(instance.modelMaximum(), 0.0);
    QCOMPARE(instance.logMessages(), QString());
    QCOMPARE(instance.modelName(), QString());
    QCOMPARE(instance.equationCount(EquationType::E), 0);
    QCOMPARE(instance.equationCount(EquationType::G), 0);
    QCOMPARE(instance.equationCount(EquationType::L), 0);
    QCOMPARE(instance.equationCount(EquationType::N), 0);
    QCOMPARE(instance.equationCount(EquationType::X), 0);
    QCOMPARE(instance.equationCount(EquationType::C), 0);
    QCOMPARE(instance.equationCount(EquationType::B), 0);
    QCOMPARE(instance.equationRowCount(), 0);
    QCOMPARE(instance.equation(0), Symbol());
    QVERIFY(instance.equations().isEmpty());
    QCOMPARE(instance.variableCount(VariableType::X), 0);
    QCOMPARE(instance.variableCount(VariableType::B), 0);
    QCOMPARE(instance.variableCount(VariableType::I), 0);
    QCOMPARE(instance.variableCount(VariableType::S1), 0);
    QCOMPARE(instance.variableCount(VariableType::S2), 0);
    QCOMPARE(instance.variableCount(VariableType::SC), 0);
    QCOMPARE(instance.variableCount(VariableType::SI), 0);
    QCOMPARE(instance.variableRowCount(), 0);
    QCOMPARE(instance.variable(0), Symbol());
    QVERIFY(instance.variables().isEmpty());
    QCOMPARE(instance.coefficents(), 0);
    QCOMPARE(instance.positiveCoefficents(), 0);
    QCOMPARE(instance.negativeCoefficents(), 0);
    QCOMPARE(instance.nonLinearCoefficents(), 0);
    QCOMPARE(instance.matrixRange(), Range());
    QCOMPARE(instance.objectiveRange(), Range());
    QCOMPARE(instance.boundsRange(), Range());
    QCOMPARE(instance.rhsRange(), Range());
    QCOMPARE(instance.longestEquationText(), QString());
    QCOMPARE(instance.longestVariableText(), QString());
    QCOMPARE(instance.maximumEquationDimension(), 0);
    QCOMPARE(instance.maximumVariableDimension(), 0);
    QCOMPARE(instance.symbols(Symbol::Equation), QVector<Symbol>());
    QCOMPARE(instance.symbols(Symbol::Variable), QVector<Symbol>());
    QCOMPARE(instance.rowCount(PredefinedViewEnum::Statistic), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::EqnAttributes), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::VarAttributes), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::Jaccobian), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::Full), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::MinMax), 0);
    QCOMPARE(instance.rowCount(PredefinedViewEnum::Unknown), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::Statistic), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::EqnAttributes), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::VarAttributes), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::Jaccobian), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::Full), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::MinMax), 0);
    QCOMPARE(instance.columnCount(PredefinedViewEnum::Unknown), 0);
    QCOMPARE(instance.data(0, 1, 4), QVariant());
    QCOMPARE(instance.headerData(1, 0, Qt::Horizontal), QString());
    QCOMPARE(instance.headerData(2, 1, Qt::Vertical), QString());
    QCOMPARE(instance.headerData(0, Qt::Horizontal, 2), 0);
    QCOMPARE(instance.headerData(1, Qt::Vertical, 2), 0);
    QList<SearchResult> result;
    instance.searchHeaderData(0, 0 , "e", false,
                              DataSource::EquationData, Qt::Horizontal, result);
    QVERIFY(result.isEmpty());
    instance.searchHeaderData(0, 0 , "e", false,
                              DataSource::VariableData, Qt::Vertical, result);
    QVERIFY(result.isEmpty());
}

void TestEmptyModelInstance::test_getSet()
{
    EmptyModelInstance instance;
    instance.setWorkspace("my_workspace");
    QCOMPARE(instance.workspace(), QDir("my_workspace").absolutePath());
    instance.setSystemDirectory("my_system_dir");
    QCOMPARE(instance.systemDirectory(), "my_system_dir");
    instance.setScratchDirectory("my_scratch_dir");
    QCOMPARE(instance.scratchDirectory(), "my_scratch_dir");
    instance.setUseOutput(true);
    QCOMPARE(instance.useOutput(), true);
}

QTEST_APPLESS_MAIN(TestEmptyModelInstance)

#include "tst_testemptymodelinstance.moc"
