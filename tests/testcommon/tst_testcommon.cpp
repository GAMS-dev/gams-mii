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
    void test_specialValues();
    void test_predefinedHeader();

    void test_searchResult();

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

void TestCommon::test_specialValues()
{
    QCOMPARE(NA, "NA");
    QCOMPARE(EPS, "EPS");
    QCOMPARE(INF, "INF");
    QCOMPARE(P_INF, "+INF");
    QCOMPARE(N_INF, "-INF");
}

void TestCommon::test_predefinedHeader()
{
    QCOMPARE(PredefinedHeader.size(), PredefinedHeaderLength);
    QCOMPARE(PredefinedHeaderLength, 5);
    QCOMPARE(PredefinedHeader.at(0), "level");
    QCOMPARE(PredefinedHeader.at(1), "lower");
    QCOMPARE(PredefinedHeader.at(2), "marginal");
    QCOMPARE(PredefinedHeader.at(3), "scale");
    QCOMPARE(PredefinedHeader.at(4), "upper");
}

void TestCommon::test_searchResult()
{
    SearchResult defaultResult;
    QCOMPARE(defaultResult.Index, -1);
    QCOMPARE(defaultResult.Orientation, Qt::Horizontal);

    SearchResult result { 5, Qt::Vertical };
    QCOMPARE(result.Index, 5);
    QCOMPARE(result.Orientation, Qt::Vertical);
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
    LabelFilter labelFilter { true, { { Qt::Horizontal, states } } };
    QCOMPARE(labelFilter.Any, true);
    QCOMPARE(labelFilter.LabelCheckStates[Qt::Horizontal], states);
}

void TestCommon::test_default_valueFilter()
{
    ValueFilter filter;
    QCOMPARE(filter.MinValue, std::numeric_limits<double>::min());
    QCOMPARE(filter.MaxValue, std::numeric_limits<double>::max());
    QCOMPARE(filter.ExcludeRange, false);
    QCOMPARE(filter.UseAbsoluteValues, false);
    QCOMPARE(filter.ShowPInf, true);
    QCOMPARE(filter.ShowNInf, true);
    QCOMPARE(filter.ShowEps, true);
    QVERIFY(filter.accepts(EPS));
    QVERIFY(filter.accepts(N_INF));
    QVERIFY(filter.accepts(P_INF));
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

    QVERIFY(!filter.accepts(EPS));
    QVERIFY(!filter.accepts(N_INF));
    QVERIFY(!filter.accepts(P_INF));
    QVERIFY(filter.accepts(1001.2));
    QVERIFY(!filter.accepts(-42));
}

QTEST_APPLESS_MAIN(TestCommon)

#include "tst_testcommon.moc"
