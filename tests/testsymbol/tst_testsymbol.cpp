#include <QtTest>

#include "symbol.h"
#include "labeltreeitem.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(LabelTreeItem)
Q_DECLARE_METATYPE(Symbol)

class TestSymbol : public QObject
{
    Q_OBJECT

public:
    TestSymbol();
    ~TestSymbol();

private slots:
    void test_default();
    void test_getSet();
    void test_comparison();
};

TestSymbol::TestSymbol()
{

}

TestSymbol::~TestSymbol()
{

}

void TestSymbol::test_default()
{
    Symbol symbol;
    QCOMPARE(symbol.offset(), -1);
    QCOMPARE(symbol.entries(), -1);
    QCOMPARE(symbol.name(), QString());
    QCOMPARE(symbol.dimension(), -1);
    QCOMPARE(symbol.type(), Symbol::Unknown);
    QCOMPARE(symbol.sectionLabels(), SectionLabels());
    QCOMPARE(symbol.label(0,0), QString());
    QCOMPARE(symbol.contains(0), false);
    QCOMPARE(symbol.isScalar(), false);
    QVERIFY(symbol.lastOffset()<0);
    QCOMPARE(symbol.firstSection(), -1);
    QVERIFY(symbol.lastSection()<0);
    QVERIFY(symbol.firstJaccSection()<0);
    QVERIFY(symbol.lastJaccSection()<0);
    QCOMPARE(symbol.labelTree(),  nullptr);
    QCOMPARE(symbol.isEquation(), false);
    QCOMPARE(symbol.isVariable(), false);
}

void TestSymbol::test_getSet()
{
    int i = 0;
    const int value = 42;
    Symbol symbol;

    symbol.setOffset(value+i);
    QCOMPARE(symbol.offset(), value+i);

    ++i;
    symbol.setEntries(value+i);
    QCOMPARE(symbol.entries(), value+i);

    symbol.setName("lala");
    QCOMPARE(symbol.name(), "lala");

    ++i;
    symbol.setDimension(value+i);
    QCOMPARE(symbol.dimension(), value+i);

    ++i;
    symbol.setType(Symbol::Equation);
    QCOMPARE(symbol.type(), Symbol::Equation);
    QVERIFY(symbol.isEquation());
    symbol.setType(Symbol::Variable);
    QCOMPARE(symbol.type(), Symbol::Variable);
    QVERIFY(symbol.isVariable());

    ++i;
    symbol.setFirstSection(value+i);
    QCOMPARE(symbol.firstSection(), value+i);
    QCOMPARE(symbol.lastSection(), value+i+symbol.entries()-1);
    QCOMPARE(symbol.firstJaccSection(), value+i-PredefinedHeaderLength);
    QCOMPARE(symbol.lastJaccSection(), symbol.lastSection()-PredefinedHeaderLength);
    QVERIFY(symbol.contains(value+i));
    QVERIFY(symbol.contains(symbol.lastSection()));

    ++i;
    QStringList data { "a", "b" };
    symbol.setLabels(value+i, data);
    QCOMPARE(symbol.label(value+i, 0), "a");
    QCOMPARE(symbol.label(value+i, 1), "b");
    QCOMPARE(symbol.label(value+i, 2), QString());
    SectionLabels sectionLabels { { value+i, data } };
    QCOMPARE(symbol.sectionLabels(), sectionLabels);

    QSharedPointer<LabelTreeItem> labelTree(new LabelTreeItem);
    symbol.setLabelTree(labelTree);
    QVERIFY(symbol.labelTree() == labelTree);
}

void TestSymbol::test_comparison()
{
    QCOMPARE(Symbol(), Symbol());
    Symbol symbol1;
    symbol1.setName("lala");
    Symbol symbol2;
    symbol2.setName("lala");
    QCOMPARE(symbol1, symbol2);
    symbol2.setDimension(42);
    QVERIFY(symbol1 != symbol2);
    QVERIFY(symbol2 != Symbol());
}

QTEST_APPLESS_MAIN(TestSymbol)

#include "tst_testsymbol.moc"
