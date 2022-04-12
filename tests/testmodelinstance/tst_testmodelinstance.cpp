#include <QtTest>

#include "modelinstance.h"

using namespace gams::studio::modelinspector;

class TestModelInstance : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestModelInstance();
    ~TestModelInstance();

private slots:
    void test_constructor();
    void test_default();
    void test_getSet();
};

TestModelInstance::TestModelInstance()
{

}

TestModelInstance::~TestModelInstance()
{

}

void TestModelInstance::test_constructor()
{
    const QString workspace = "myWorkDir";
    const QString systemDir = "mySysDir";
    const QString scratchDir = "myScratchDir";
    ModelInstance instance(workspace, systemDir, scratchDir);
    QVERIFY(!instance.isInitialized());
    auto realWorkspace = QDir(workspace).absolutePath();
    QCOMPARE(instance.workspace(), realWorkspace);
    QCOMPARE(instance.systemDirectory(), systemDir);
    QCOMPARE(instance.scratchDirectory(), scratchDir);
}

void TestModelInstance::test_default()
{
    ModelInstance instance;
    QVERIFY(!instance.isInitialized());
}

void TestModelInstance::test_getSet()
{

}

QTEST_APPLESS_MAIN(TestModelInstance)

#include "tst_testmodelinstance.moc"
