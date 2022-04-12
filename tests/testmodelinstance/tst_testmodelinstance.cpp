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
    void test_defaultConstructor();

};

TestModelInstance::TestModelInstance()
{

}

TestModelInstance::~TestModelInstance()
{

}

void TestModelInstance::test_defaultConstructor()
{
    ModelInstance instance;
    QVERIFY(!instance.isInitialized());
}

QTEST_APPLESS_MAIN(TestModelInstance)

#include "tst_testmodelinstance.moc"
