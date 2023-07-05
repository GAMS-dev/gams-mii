/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
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
