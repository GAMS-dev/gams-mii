#include <QtTest>

#include "aggregation.h"

using namespace gams::studio::modelinspector;

//Q_DECLARE_METATYPE(Aggregation)
//Q_DECLARE_METATYPE(AggregationItem)
//Q_DECLARE_METATYPE(AggregationSymbols)
//Q_DECLARE_METATYPE(AggregationMap)
//Q_DECLARE_METATYPE(IdentifierFilter)
//Q_DECLARE_METATYPE(ValueFilter)

class TestAggregation : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestAggregation();
    ~TestAggregation();

private slots:
    void initTestCase();

    void test_default_aggregationItem();
    void test_getSet_aggregationItem();
    void test_domainLabel_aggregationItem();

    void test_default_aggregation();
    void test_getSet_aggregation();

    void test_default_aggregator();

    void test_aggregateLabels_aggregator();
    void test_aggregateLabels_aggregator_minmax();
};

TestAggregation::TestAggregation()
{

}

TestAggregation::~TestAggregation()
{

}

void TestAggregation::initTestCase()
{

}

void TestAggregation::test_default_aggregationItem()
{
    AggregationItem item;
    QCOMPARE(item.text(), QString());
    QCOMPARE(item.symbolIndex(), -1);
    QVERIFY(item.checkStates().isEmpty());
    QCOMPARE(item.aggregatedLabelTree(), nullptr);
    QCOMPARE(item.label(-1, -1), QString());
    QCOMPARE(item.label(0, 0), QString());
    QCOMPARE(item.label(1, 1), QString());
    QCOMPARE(item.visibleSectionCount(), -1);
    QCOMPARE(item.isSectionVisible(-1), false);
    QCOMPARE(item.isSectionVisible(0), false);
    QCOMPARE(item.isSectionVisible(1), false);
    QVERIFY(item.visibleSections().isEmpty());
    QVERIFY(item.unitedSections().isEmpty());
}

void TestAggregation::test_getSet_aggregationItem()
{

}

void TestAggregation::test_domainLabel_aggregationItem()
{
    AggregationItem item;
    DomainLabels dLabels { "i", "j"};
    item.setDomainLabels(dLabels);
    QCOMPARE(item.domainLabel(-1), QString());
    QCOMPARE(item.domainLabel(0), dLabels.at(0));
    QCOMPARE(item.domainLabel(1), dLabels.at(1));
    QCOMPARE(item.domainLabel(2), QString());
}

void TestAggregation::test_default_aggregation()
{
    Aggregation aggregation;
    QCOMPARE(aggregation.useAbsoluteValues(), false);
    QCOMPARE(aggregation.type(), Aggregation::None);
    QCOMPARE(aggregation.typeText(), "None");
    QVERIFY(aggregation.aggregationMap().isEmpty());
    QVERIFY(aggregation.aggregationSymbols(Qt::Horizontal).isEmpty());
    QVERIFY(aggregation.aggregationSymbols(Qt::Vertical).isEmpty());
    QVERIFY(aggregation.identifierFilter().isEmpty());
    QCOMPARE(aggregation.valueFilter().MinValue, std::numeric_limits<double>::lowest());
    QCOMPARE(aggregation.valueFilter().MaxValue, std::numeric_limits<double>::max());
    QCOMPARE(aggregation.valueFilter().ExcludeRange, false);
    QCOMPARE(aggregation.valueFilter().UseAbsoluteValues, false);
    QVERIFY(aggregation.valueFilter().accepts("EPS"));
    QVERIFY(aggregation.valueFilter().accepts("+INF"));
    QVERIFY(aggregation.valueFilter().accepts("-INF"));
}

void TestAggregation::test_getSet_aggregation()
{

}

void TestAggregation::test_default_aggregator()
{

}

void TestAggregation::test_aggregateLabels_aggregator()
{

}

void TestAggregation::test_aggregateLabels_aggregator_minmax()
{

}

QTEST_APPLESS_MAIN(TestAggregation)

#include "tst_testaggregation.moc"
