#include <QtTest>

#include "common.h"

using namespace gams::studio::modelinspector;

class TestCommon : public QObject
{
    Q_OBJECT

public:
    TestCommon();
    ~TestCommon();

private slots:
    void test_Mi_specialValues();
    void test_Mi_isSpecialValue();
    void test_Mi_roleNames();
    void test_Mi_isAggregatable();
    void test_Mi_isStandardView();

    void test_searchResult();
    void test_searchOperators();

    void test_default_identifierState();
    void test_getSet_identifierState();
    void test_unite_identifierState();

    void test_labelFilter();

    void test_default_valueFilter();
    void test_getSet_valueFilter();
};

TestCommon::TestCommon()
{

}

TestCommon::~TestCommon()
{

}

void TestCommon::test_Mi_specialValues()
{
    QCOMPARE(Mi::SV_NA, "NA");
    QCOMPARE(Mi::SV_EPS, "EPS");
    QCOMPARE(Mi::SV_INF, "INF");
    QCOMPARE(Mi::SV_PINF, "+INF");
    QCOMPARE(Mi::SV_NINF, "-INF");
}

void TestCommon::test_Mi_isSpecialValue()
{
    QCOMPARE(Mi::isSpecialValue(Mi::SV_NA), true);
    QCOMPARE(Mi::isSpecialValue(Mi::SV_EPS), true);
    QCOMPARE(Mi::isSpecialValue(Mi::SV_INF), true);
    QCOMPARE(Mi::isSpecialValue(Mi::SV_PINF), true);
    QCOMPARE(Mi::isSpecialValue(Mi::SV_NINF), true);
    QCOMPARE(Mi::isSpecialValue(QString()), false);
    QCOMPARE(Mi::isSpecialValue("stuff"), false);
    QCOMPARE(Mi::isSpecialValue(QString::number(0.1)), false);
}

void TestCommon::test_Mi_roleNames()
{
    auto names = Mi::roleNames();
    QCOMPARE(names.size(), 7);
    QVERIFY(names.contains(Mi::IndexDataRole));
    QVERIFY(names.contains(Mi::LabelDataRole));
    QVERIFY(names.contains(Mi::RowEntryRole));
    QVERIFY(names.contains(Mi::ColumnEntryRole));
    QVERIFY(names.contains(Mi::HorizontalDimensionRole));
    QVERIFY(names.contains(Mi::VerticalDimensionRole));
    QVERIFY(names.contains(Mi::SectionLabelRole));
    QCOMPARE(names[Mi::IndexDataRole], "indexdata");
    QCOMPARE(names[Mi::LabelDataRole], "labeldata");
    QCOMPARE(names[Mi::RowEntryRole], "rowentry");
    QCOMPARE(names[Mi::ColumnEntryRole], "columnentry");
    QCOMPARE(names[Mi::HorizontalDimensionRole], "horizontaldimension");
    QCOMPARE(names[Mi::VerticalDimensionRole], "verticaldimension");
    QCOMPARE(names[Mi::SectionLabelRole], "sectionlabel");
}

void TestCommon::test_Mi_isAggregatable()
{
    QCOMPARE(Mi::isAggregatable(ViewDataType::Jaccobian), true);
    QCOMPARE(Mi::isAggregatable(ViewDataType::BP_Overview), false);
    QCOMPARE(Mi::isAggregatable(ViewDataType::BP_Count), false);
    QCOMPARE(Mi::isAggregatable(ViewDataType::BP_Average), false);
    QCOMPARE(Mi::isAggregatable(ViewDataType::BP_Scaling), false);
    QCOMPARE(Mi::isAggregatable(ViewDataType::Postopt), false);
    QCOMPARE(Mi::isAggregatable(ViewDataType::Symbols), true);
    QCOMPARE(Mi::isAggregatable(ViewDataType::Unknown), false);
}

void TestCommon::test_Mi_isStandardView()
{
    QCOMPARE(Mi::isStandardView(ViewDataType::Jaccobian), true);
    QCOMPARE(Mi::isStandardView(ViewDataType::BP_Overview), true);
    QCOMPARE(Mi::isStandardView(ViewDataType::BP_Count), false);
    QCOMPARE(Mi::isStandardView(ViewDataType::BP_Average), false);
    QCOMPARE(Mi::isStandardView(ViewDataType::BP_Scaling), false);
    QCOMPARE(Mi::isStandardView(ViewDataType::Postopt), true);
    QCOMPARE(Mi::isStandardView(ViewDataType::Symbols), true);
    QCOMPARE(Mi::isStandardView(ViewDataType::Unknown), true);
}

void TestCommon::test_searchResult()
{
    SearchResult result;
    QCOMPARE(result.Term, QString());
    QCOMPARE(result.IsRegEx, false);
    QVERIFY(result.Entries.isEmpty());

    result.Term = "x";
    result.IsRegEx = true;
    result.Entries.append(SearchResult::SearchEntry { 0, Qt::Horizontal });
    QCOMPARE(result.Term, "x");
    QCOMPARE(result.IsRegEx, true);
    QVERIFY(!result.Entries.isEmpty());

    SearchResult::SearchEntry entry;
    QCOMPARE(entry.Index, -1);
    QCOMPARE(entry.Orientation, Qt::Horizontal);

    entry.Index = 42;
    entry.Orientation = Qt::Vertical;
    QCOMPARE(entry.Index, 42);
    QCOMPARE(entry.Orientation, Qt::Vertical);
}

void TestCommon::test_searchOperators()
{
    SearchResult result1 { "topeka", true, {} };
    QCOMPARE(result1, result1);
    SearchResult result2 { "topeka", true, {} };
    QCOMPARE(result1, result2);
    SearchResult result3 { "seatle", false, {} };
    QVERIFY(result2 != result3);

    SearchResult::SearchEntry entry1 { 0, Qt::Horizontal };
    QCOMPARE(entry1, entry1);
    SearchResult::SearchEntry entry2 { 0, Qt::Horizontal };
    QCOMPARE(entry1, entry2);
    SearchResult::SearchEntry entry3 { 1, Qt::Vertical};
    QVERIFY(entry1 != entry3);

    result1.Entries.append(entry1);
    QCOMPARE(result1, result1);
    result2.Entries.append(entry2);
    QCOMPARE(result1, result2);
    QVERIFY(result2 != result3);
    result3.Entries.append(entry2);
    result3.Entries.append(entry3);
    QVERIFY(result1 != result3);
}

void TestCommon::test_default_identifierState()
{
    IdentifierState state;
    QCOMPARE(state.Enabled, false);
    QCOMPARE(state.SectionIndex, -1);
    QCOMPARE(state.SymbolIndex, -1);
    QCOMPARE(state.Text, QString());
    QCOMPARE(state.Checked, Qt::Unchecked);
    QCOMPARE(state.CheckStates, IndexCheckStates());
    QVERIFY(!state.isValid());
    QVERIFY(!state.disabled());
}

void TestCommon::test_getSet_identifierState()
{
    IdentifierState state;

    state.Enabled = true;
    QCOMPARE(state.Enabled, true);

    state.SectionIndex = 4;
    QCOMPARE(state.SectionIndex, 4);

    state.SymbolIndex = 2;
    QCOMPARE(state.SymbolIndex, 2);

    state.Text = "x";
    QCOMPARE(state.Text, "x");

    state.Checked = Qt::Checked;
    QCOMPARE(state.Checked, Qt::Checked);

    IndexCheckStates checkStates { { 0, Qt::Unchecked },
                                  { 0, Qt::Unchecked },
                                  { 0, Qt::Unchecked } };
    state.CheckStates = checkStates;
    QCOMPARE(state.CheckStates, checkStates);

    QVERIFY(state.isValid());
    QVERIFY(state.disabled());
}

void TestCommon::test_unite_identifierState()
{
    IdentifierState state_1;
    IndexCheckStates checkStates { { 0, Qt::Unchecked },
                                  { 0, Qt::Unchecked },
                                  { 0, Qt::Unchecked } };
    IdentifierState state_2;
    state_2.SectionIndex = 4;
    state_2.CheckStates = checkStates;

    state_1.unite(state_2);
    QCOMPARE(state_1.SectionIndex, 4);
    QCOMPARE(state_1.CheckStates, checkStates);
}

void TestCommon::test_labelFilter()
{
    LabelFilter defaultFilter;
    QCOMPARE(defaultFilter.Any, false);
    QVERIFY(defaultFilter.LabelCheckStates.isEmpty());

    LabelCheckStates states = { { "l1", Qt::Checked},
                                { "l2", Qt::Unchecked } };
    LabelFilter labelFilter { true,
                              { { Qt::Horizontal, states } } };
    QCOMPARE(labelFilter.Any, true);
    QCOMPARE(labelFilter.LabelCheckStates[Qt::Horizontal], states);
}

void TestCommon::test_default_valueFilter()
{
    ValueFilter filter;
    QCOMPARE(filter.MinValue, std::numeric_limits<double>::lowest());
    QCOMPARE(filter.MaxValue, std::numeric_limits<double>::max());
    QCOMPARE(filter.ExcludeRange, false);
    QCOMPARE(filter.UseAbsoluteValues, false);
    QCOMPARE(filter.ShowPInf, true);
    QCOMPARE(filter.ShowNInf, true);
    QCOMPARE(filter.ShowEps, true);
    QVERIFY(filter.accepts(Mi::SV_EPS));
    QVERIFY(filter.accepts(Mi::SV_NINF));
    QVERIFY(filter.accepts(Mi::SV_PINF));
    QVERIFY(filter.accepts(std::numeric_limits<double>::min()));
    QVERIFY(filter.accepts(std::numeric_limits<double>::max()));
}

void TestCommon::test_getSet_valueFilter()
{
    ValueFilter filter;

    filter.MinValue = -42;
    QCOMPARE(filter.MinValue, -42);

    filter.MaxValue = 42;
    QCOMPARE(filter.MaxValue, 42);

    filter.ExcludeRange = true;
    QCOMPARE(filter.ExcludeRange, true);

    filter.UseAbsoluteValues = true;
    QCOMPARE(filter.UseAbsoluteValues, true);

    filter.ShowPInf = false;
    QCOMPARE(filter.ShowPInf, false);

    filter.ShowNInf = false;
    QCOMPARE(filter.ShowNInf, false);

    filter.ShowEps = false;
    QCOMPARE(filter.ShowEps, false);

    QVERIFY(!filter.accepts(Mi::SV_EPS));
    QVERIFY(!filter.accepts(Mi::SV_NINF));
    QVERIFY(!filter.accepts(Mi::SV_PINF));
    QVERIFY(filter.accepts(1001.2));
    QVERIFY(!filter.accepts(-42));
}

QTEST_APPLESS_MAIN(TestCommon)

#include "tst_testcommon.moc"
