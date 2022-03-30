#include <QtTest>

// add necessary includes here

class TestModelInstance : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestModelInstance();
    ~TestModelInstance();

private slots:
    void test_case1();

};

TestModelInstance::TestModelInstance()
{

}

TestModelInstance::~TestModelInstance()
{

}

void TestModelInstance::test_case1()
{

}

QTEST_APPLESS_MAIN(TestModelInstance)

#include "tst_testmodelinstance.moc"
