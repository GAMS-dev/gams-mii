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

#include "aggregation.h"
#include "labeltreeitem.h"

using namespace gams::studio::mii;

//Q_DECLARE_METATYPE(Aggregation)
//Q_DECLARE_METATYPE(AggregationItem)
//Q_DECLARE_METATYPE(AggregationSymbols)
//Q_DECLARE_METATYPE(AggregationMap)
//Q_DECLARE_METATYPE(IdentifierFilter)
//Q_DECLARE_METATYPE(ValueFilter)

class TestAggregation : public QObject
{
    Q_OBJECT

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
    AggregationItem item;
    item.setText("Root");
    QCOMPARE(item.text(), "Root");
    item.setSymbolIndex(42);
    QCOMPARE(item.symbolIndex(), 42);
    item.setCheckState(1, Qt::Checked);
    item.setCheckState(2, Qt::Unchecked);
    QVERIFY(item.isChecked(1));
    QVERIFY(!item.isChecked(2));
    QSharedPointer<LabelTreeItem> treeItem = QSharedPointer<LabelTreeItem>(new LabelTreeItem("TreeRoot"));
    item.setAggregatedLabelTree(treeItem);
    QCOMPARE(item.aggregatedLabelTree(), treeItem);
    SectionLabels labels { {0, {"a", "b"} },
                           {1, {"c", "d"} } };
    item.setLabels(labels);
    QCOMPARE(item.labels(), labels);
    QList<int> sections {1, 2, 3};
    item.setMappedSections(sections);
    QCOMPARE(item.mappedSections(), sections);
    item.setVisibleSectionCount(22);
    QCOMPARE(item.visibleSectionCount(), 22);
    QList<int> visibleSections {4, 5, 6};
    item.setVisibleSections(visibleSections);
    QCOMPARE(item.visibleSections(), visibleSections);
    UnitedSections unitedSections { {1, 2, 3}, {4, 5, 6} };
    item.setUnitedSections(unitedSections);
    QCOMPARE(item.unitedSections(), unitedSections);
    DomainLabels domainLabels { "d1", "d2", "d3" };
    item.setDomainLabels(domainLabels);
    QCOMPARE(item.domainLabel(0), domainLabels.at(0));
    QCOMPARE(item.domainLabel(1), domainLabels.at(1));
    QCOMPARE(item.domainLabel(2), domainLabels.at(2));
    item.setScalar(true);
    QVERIFY(item.isScalar());
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
}

void TestAggregation::test_getSet_aggregation()
{
    Aggregation aggregation;
    aggregation.setUseAbsoluteValues(true);
    QCOMPARE(aggregation.useAbsoluteValues(), true);
    aggregation.setType("Count");
    QCOMPARE(aggregation.type(), Aggregation::Count);
    QCOMPARE(aggregation.typeText(), "Count");

    // rest...
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
