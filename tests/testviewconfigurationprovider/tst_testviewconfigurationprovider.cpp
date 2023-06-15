#include <QtTest>

#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"

using namespace gams::studio::modelinspector;

class TestViewConfigurationProvider : public QObject
{
    Q_OBJECT

public:
    TestViewConfigurationProvider();
    ~TestViewConfigurationProvider();

private slots:
    void test_defaultConfiguration();
    void test_configuration();

private:
    void test_viewConfiguration(AbstractViewConfiguration *viewConfig,
                                ViewDataType type);
};

TestViewConfigurationProvider::TestViewConfigurationProvider()
{

}

TestViewConfigurationProvider::~TestViewConfigurationProvider()
{

}

void TestViewConfigurationProvider::test_defaultConfiguration()
{
    auto viewConfig = ViewConfigurationProvider::defaultConfiguration();
    test_viewConfiguration(viewConfig, ViewDataType::Unknown);
}

void TestViewConfigurationProvider::test_configuration()
{
    AbstractViewConfiguration* viewConfig = nullptr;
    auto modelInstance = QSharedPointer<AbstractModelInstance>(new EmptyModelInstance);

    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::Unknown, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::Unknown);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::BP_Overview, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::BP_Overview);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::BP_Count, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::BP_Count);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::BP_Average, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::BP_Average);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::BP_Scaling, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::BP_Scaling);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::Jaccobian, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::Jaccobian);
    delete viewConfig;

    viewConfig = nullptr;
    viewConfig = ViewConfigurationProvider::configuration(ViewDataType::Symbols, modelInstance);
    test_viewConfiguration(viewConfig, ViewDataType::Symbols);
    delete viewConfig;
}

void TestViewConfigurationProvider::test_viewConfiguration(AbstractViewConfiguration *viewConfig,
                                                           ViewDataType type)
{
    if (viewConfig) {
        QVERIFY(true);
    } else {
        QVERIFY(false);
        return;
    }
    QVERIFY(viewConfig->modelInstance() != nullptr);
    QCOMPARE(viewConfig->viewType(), type);
    QCOMPARE(viewConfig->view(), (int)type);

    //QCOMPARE(viewConfig->currentValueFilter(), ValueFilter());
    //QCOMPARE(viewConfig->defaultValueFilter(), ValueFilter());
    QCOMPARE(viewConfig->currentValueFilter(), viewConfig->defaultValueFilter());
    QCOMPARE(viewConfig->searchResult(), SearchResult());
}

QTEST_APPLESS_MAIN(TestViewConfigurationProvider)

#include "tst_testviewconfigurationprovider.moc"
