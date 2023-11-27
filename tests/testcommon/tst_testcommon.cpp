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

#include "common.h"

using namespace gams::studio::mii;

class TestCommon : public QObject
{
    Q_OBJECT

public:
    TestCommon();
    ~TestCommon();

private slots:
    void test_Mi_roleNames();

    void test_searchResult();
    void test_searchOperators();

    void test_default_identifierState();
    void test_getSet_identifierState();
    void test_unite_identifierState();
    void test_operators_identifierState();

    void test_labelFilter();

    void test_default_valueFilter();
    void test_getSet_valueFilter();

    void test_AttributeHelper_attributeText();
    void test_AttributeHelper_attributeValue();
    void test_AttributeHelper_static();

    void test_SpecialValueHelper_specialValueText();
    void test_SpecialValueHelper_isSpecialValue();
    void test_SpecialValueHelper_static();

    void test_ViewHelper_isAggregatable();
    void test_ViewHelper_isStandardView();
    void test_ViewHelper_zoomFactor();
    void test_ViewHelper_static();
};

TestCommon::TestCommon()
{

}

TestCommon::~TestCommon()
{

}

void TestCommon::test_Mi_roleNames()
{
    auto names = ViewHelper::roleNames();
    QCOMPARE(names.size(), 6);
    QVERIFY(names.contains(ViewHelper::ItemDataRole::IndexDataRole));
    QVERIFY(names.contains(ViewHelper::ItemDataRole::LabelDataRole));
    QVERIFY(names.contains(ViewHelper::ItemDataRole::RowEntryRole));
    QVERIFY(names.contains(ViewHelper::ItemDataRole::ColumnEntryRole));
    QVERIFY(names.contains(ViewHelper::ItemDataRole::DimensionRole));
    QVERIFY(names.contains(ViewHelper::ItemDataRole::SectionLabelRole));
    QCOMPARE(names[ViewHelper::ItemDataRole::IndexDataRole], "indexdata");
    QCOMPARE(names[ViewHelper::ItemDataRole::LabelDataRole], "labeldata");
    QCOMPARE(names[ViewHelper::ItemDataRole::RowEntryRole], "rowentry");
    QCOMPARE(names[ViewHelper::ItemDataRole::ColumnEntryRole], "columnentry");
    QCOMPARE(names[ViewHelper::ItemDataRole::DimensionRole], "dimension");
    QCOMPARE(names[ViewHelper::ItemDataRole::SectionLabelRole], "sectionlabel");
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

void TestCommon::test_operators_identifierState()
{
    IdentifierState state;
    state.Enabled = true;
    state.SectionIndex = 4;
    state.SymbolIndex = 2;
    state.Text = "x";
    state.Checked = Qt::Checked;
    IndexCheckStates checkStates { { 0, Qt::Unchecked },
                                 { 0, Qt::Unchecked },
                                 { 0, Qt::Unchecked } };
    state.CheckStates = checkStates;

    QCOMPARE(IdentifierState(), IdentifierState());
    QVERIFY(state != IdentifierState());
    QCOMPARE(state, state);

    IdentifierState state0(state);
    QCOMPARE(state, state0);
    IdentifierState state1;
    state1 = state;
    QCOMPARE(state, state1);
}

void TestCommon::test_labelFilter()
{
    LabelFilter defaultFilter;
    QCOMPARE(defaultFilter.Any, false);
    QVERIFY(defaultFilter.LabelCheckStates.isEmpty());

    LabelCheckStates states = { { "l1", Qt::Checked},
                                { "l2", Qt::Unchecked } };
    QCOMPARE(states, states);
    QVERIFY(states != LabelCheckStates());

    LabelFilter labelFilter { true,
                              { { Qt::Horizontal, states } } };
    QCOMPARE(labelFilter.Any, true);
    QCOMPARE(labelFilter.LabelCheckStates[Qt::Horizontal], states);
    QCOMPARE(labelFilter, labelFilter);
    QVERIFY(labelFilter != LabelFilter());
    defaultFilter = labelFilter;
    QCOMPARE(defaultFilter, labelFilter);
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
    QVERIFY(filter.accepts(ValueHelper::EPSText));
    QVERIFY(filter.accepts(ValueHelper::NINFText));
    QVERIFY(filter.accepts(ValueHelper::PINFText));
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
    
    QVERIFY(!filter.accepts(ValueHelper::EPSText));
    QVERIFY(!filter.accepts(ValueHelper::NINFText));
    QVERIFY(!filter.accepts(ValueHelper::PINFText));
    QVERIFY(filter.accepts(1001.2));
    QVERIFY(!filter.accepts(-42));
}

void TestCommon::test_AttributeHelper_attributeText()
{
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Level), "Level");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Marginal), "Marginal");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::MarginalNum), "MarginalNum");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Lower), "Lower");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Upper), "Upper");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Scale), "Scale");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Range), "Range");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::SlackLB), "Slack lower bound");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::SlackUB), "Slack upper bound");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Slack), "Slack");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Infeasibility), "Infeasibility");
    QCOMPARE(AttributeHelper::attributeText(AttributeHelper::Type), "Type");
}

void TestCommon::test_AttributeHelper_attributeValue()
{
    double pInf =  1e+299;
    double nInf = -1e+299;
    QCOMPARE(AttributeHelper::attributeValue(pInf, nInf, true, true), 0.0);
    QCOMPARE(AttributeHelper::attributeValue(nInf, pInf, true, true), 0.0);
    QCOMPARE(AttributeHelper::attributeValue(nInf, nInf, true, true), nInf);
    QCOMPARE(AttributeHelper::attributeValue(pInf, pInf, true, true), pInf);
    QCOMPARE(AttributeHelper::attributeValue(pInf, 42, true, false), pInf);
    QCOMPARE(AttributeHelper::attributeValue(42, pInf, false, true), pInf);
    QCOMPARE(AttributeHelper::attributeValue(38, nInf, false, true), nInf);
    QCOMPARE(AttributeHelper::attributeValue(nInf, 38, true, false), nInf);
    QCOMPARE(AttributeHelper::attributeValue(8, 8), 0.0);
    QCOMPARE(AttributeHelper::attributeValue(4, 8), -4);
}

void TestCommon::test_AttributeHelper_static()
{
    QCOMPARE(AttributeHelper::LevelText, "Level");
    QCOMPARE(AttributeHelper::MarginalText, "Marginal");
    QCOMPARE(AttributeHelper::MarginalNumText, "MarginalNum");
    QCOMPARE(AttributeHelper::LowerText, "Lower");
    QCOMPARE(AttributeHelper::UpperText, "Upper");
    QCOMPARE(AttributeHelper::ScaleText, "Scale");
    QCOMPARE(AttributeHelper::RangeText, "Range");
    QCOMPARE(AttributeHelper::SlackLBText, "Slack lower bound");
    QCOMPARE(AttributeHelper::SlackUBText, "Slack upper bound");
    QCOMPARE(AttributeHelper::SlackText, "Slack");
    QCOMPARE(AttributeHelper::InfeasibilityText, "Infeasibility");
    QCOMPARE(AttributeHelper::TypeText, "Type");
}

void TestCommon::test_SpecialValueHelper_specialValueText()
{
    QCOMPARE(ValueHelper::specialValueText(ValueHelper::SpecialValueType::NA), "NA");
    QCOMPARE(ValueHelper::specialValueText(ValueHelper::SpecialValueType::EPS), "EPS");
    QCOMPARE(ValueHelper::specialValueText(ValueHelper::SpecialValueType::INF), "INF");
    QCOMPARE(ValueHelper::specialValueText(ValueHelper::SpecialValueType::P_INF), "+INF");
    QCOMPARE(ValueHelper::specialValueText(ValueHelper::SpecialValueType::N_INF), "-INF");
}

void TestCommon::test_SpecialValueHelper_isSpecialValue()
{
    QCOMPARE(ValueHelper::isSpecialValue(ValueHelper::NAText), true);
    QCOMPARE(ValueHelper::isSpecialValue(ValueHelper::EPSText), true);
    QCOMPARE(ValueHelper::isSpecialValue(ValueHelper::INFText), true);
    QCOMPARE(ValueHelper::isSpecialValue(ValueHelper::PINFText), true);
    QCOMPARE(ValueHelper::isSpecialValue(ValueHelper::NINFText), true);
    QCOMPARE(ValueHelper::isSpecialValue(QString()), false);
    QCOMPARE(ValueHelper::isSpecialValue("stuff"), false);
    QCOMPARE(ValueHelper::isSpecialValue(QString::number(0.1)), false);
}

void TestCommon::test_SpecialValueHelper_static()
{
    QCOMPARE(ValueHelper::NAText, "NA");
    QCOMPARE(ValueHelper::EPSText, "EPS");
    QCOMPARE(ValueHelper::INFText, "INF");
    QCOMPARE(ValueHelper::PINFText, "+INF");
    QCOMPARE(ValueHelper::NINFText, "-INF");
}

void TestCommon::test_ViewHelper_isAggregatable()
{
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::BP_Overview), false);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::BP_Count), false);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::BP_Average), false);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::BP_Scaling), false);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::Postopt), false);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::Symbols), true);
    QCOMPARE(ViewHelper::isAggregatable(ViewHelper::ViewDataType::Unknown), false);
}

void TestCommon::test_ViewHelper_isStandardView()
{
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::BP_Overview), true);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::BP_Count), false);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::BP_Average), false);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::BP_Scaling), false);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::Postopt), true);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::Symbols), true);
    QCOMPARE(ViewHelper::isStandardView(ViewHelper::ViewDataType::Unknown), true);
}

void TestCommon::test_ViewHelper_zoomFactor()
{
    QCOMPARE(ViewHelper::ZoomFactor, 2);
}

void TestCommon::test_ViewHelper_static()
{
    QCOMPARE(ViewHelper::AttributeHeaderText, "Attributes");
    QCOMPARE(ViewHelper::EquationHeaderText, "Equations");
    QCOMPARE(ViewHelper::VariableHeaderText, "Variables");
}

QTEST_APPLESS_MAIN(TestCommon)

#include "tst_testcommon.moc"
