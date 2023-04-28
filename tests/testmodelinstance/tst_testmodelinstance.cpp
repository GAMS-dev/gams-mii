#include <QtTest>

#include "modelinstance.h"

using namespace gams::studio::modelinspector;

class TestModelInstance : public QObject
{// TODO tests... also mind error state/messages
    Q_OBJECT

public:
    TestModelInstance();
    ~TestModelInstance();

private slots:
    void test_constructor_initialize();
    void test_default();
    void test_getSet();
};

TestModelInstance::TestModelInstance()
{

}

TestModelInstance::~TestModelInstance()
{

}

void TestModelInstance::test_constructor_initialize()
{
    const QString workspace = "myWorkDir";
    const QString systemDir = "mySysDir";
    const QString scratchDir = "myScratchDir";
    ModelInstance instance(workspace, systemDir, scratchDir);
    QVERIFY(!instance.logMessages().isEmpty());
    auto realWorkspace = QDir(workspace).absolutePath();
    QCOMPARE(instance.workspace(), realWorkspace);
    QCOMPARE(instance.systemDirectory(), systemDir);
    QCOMPARE(instance.scratchDirectory(), scratchDir);
}

void TestModelInstance::test_default()
{
    ModelInstance instance;
    QVERIFY(!instance.logMessages().isEmpty());
    QCOMPARE(instance.workspace(), QDir(".").absolutePath());
    QCOMPARE(instance.systemDirectory(), QString());
    QCOMPARE(instance.scratchDirectory(), QString());
    QCOMPARE(instance.useOutput(), false);
#if defined(__unix__)
    QCOMPARE(instance.modelName(), QString());
#else
    // Needed because of the GAMS Windows registry magic
    QCOMPARE(instance.modelName(), "GAMS Model");
#endif
}

void TestModelInstance::test_getSet()
{
    ModelInstance instance;
    QVERIFY(!instance.logMessages().isEmpty());
    instance.setWorkspace("my_workspace");
    QCOMPARE(instance.workspace(), QDir("my_workspace").absolutePath());
    instance.setSystemDirectory("my_system_dir");
    QCOMPARE(instance.systemDirectory(), "my_system_dir");
    instance.setScratchDirectory("my_scratch_dir");
    QCOMPARE(instance.scratchDirectory(), "my_scratch_dir");
    instance.setUseOutput(true);
    QCOMPARE(instance.useOutput(), true);
}

QTEST_APPLESS_MAIN(TestModelInstance)

#include "tst_testmodelinstance.moc"
