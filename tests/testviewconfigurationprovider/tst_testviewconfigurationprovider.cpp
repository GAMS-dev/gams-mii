#include <QtTest>

// add necessary includes here

class TestViewConfigurationProvider : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestViewConfigurationProvider();
    ~TestViewConfigurationProvider();

private slots:
    void test_case1();

};

TestViewConfigurationProvider::TestViewConfigurationProvider()
{

}

TestViewConfigurationProvider::~TestViewConfigurationProvider()
{

}

void TestViewConfigurationProvider::test_case1()
{

}

QTEST_APPLESS_MAIN(TestViewConfigurationProvider)

#include "tst_testviewconfigurationprovider.moc"
