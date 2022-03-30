#include <QtTest>

// add necessary includes here

class TestDataHandler : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestDataHandler();
    ~TestDataHandler();

private slots:
    void test_case1();

};

TestDataHandler::TestDataHandler()
{

}

TestDataHandler::~TestDataHandler()
{

}

void TestDataHandler::test_case1()
{

}

QTEST_APPLESS_MAIN(TestDataHandler)

#include "tst_testdatahandler.moc"
