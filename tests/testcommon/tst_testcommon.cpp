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
    void test_predefinedHeader();

    void test_symbolUelFilterItemBlank();
    void test_symbolUelFilterItemGetSet();

    void test_valueFilterBlank();
    void test_valueFilterGetSet();
};

TestCommon::TestCommon()
{

}

TestCommon::~TestCommon()
{

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

void TestCommon::test_symbolUelFilterItemBlank()
{
    //SymbolUelFilterItem filter;
    //QCOMPARE(filter.EntireSymbol, false);
    //QCOMPARE(filter.SectionIndex, -1);
    //QCOMPARE(filter.SymbolIndex, -1);
    //QVERIFY(filter.CheckState.isEmpty());
    //QCOMPARE(filter.isValid(), false);
}

void TestCommon::test_symbolUelFilterItemGetSet()
{
    //SymbolUelFilterItem filter;
    //
    //filter.EntireSymbol = true;
    //QCOMPARE(filter.EntireSymbol, true);
    //
    //filter.SectionIndex = 0;
    //QCOMPARE(filter.SectionIndex, 0);
    //
    //filter.SymbolIndex = 1;
    //QCOMPARE(filter.SymbolIndex, 1);
    //
    //filter.CheckState[0] = Qt::Checked;
    //QCOMPARE(filter.CheckState[0], Qt::Checked);
    //
    //QVERIFY(filter.isValid());
}

void TestCommon::test_valueFilterBlank()
{
    ValueFilter filter;
    QCOMPARE(filter.MinValue, std::numeric_limits<double>::min());
    QCOMPARE(filter.MaxValue, std::numeric_limits<double>::max());
    QCOMPARE(filter.Exclude, false);
    QCOMPARE(filter.ShowPInf, true);
    QCOMPARE(filter.ShowNInf, true);
    QCOMPARE(filter.ShowEps, true);
}

void TestCommon::test_valueFilterGetSet()
{
    ValueFilter filter;

    filter.MinValue = -42;
    QCOMPARE(filter.MinValue, -42);

    filter.MaxValue = 42;
    QCOMPARE(filter.MaxValue, 42);

    filter.Exclude = true;
    QCOMPARE(filter.Exclude, true);

    filter.ShowPInf = false;
    QCOMPARE(filter.ShowPInf, false);

    filter.ShowNInf = false;
    QCOMPARE(filter.ShowNInf, false);

    filter.ShowEps = false;
    QCOMPARE(filter.ShowEps, false);
}

QTEST_APPLESS_MAIN(TestCommon)

#include "tst_testcommon.moc"
