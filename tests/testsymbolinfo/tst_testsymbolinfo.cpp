#include <QtTest>

#include "symbolinfo.h"
#include "labeltreeitem.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(LabelTreeItem)
Q_DECLARE_METATYPE(SymbolInfo)

class TestSymbolInfo : public QObject
{
    Q_OBJECT

public:
    TestSymbolInfo();
    ~TestSymbolInfo();

private slots:
    void test_default();
    void test_getSet();
};

TestSymbolInfo::TestSymbolInfo()
{

}

TestSymbolInfo::~TestSymbolInfo()
{

}

void TestSymbolInfo::test_default()
{
    SymbolInfo symInfo;
    QCOMPARE(symInfo.offset(), -1);
    QCOMPARE(symInfo.entries(), -1);
    QCOMPARE(symInfo.name(), QString());
    QCOMPARE(symInfo.dimension(), -1);
    QCOMPARE(symInfo.type(), -1);
    QCOMPARE(symInfo.sectionLabels(), SectionLabels());
    QCOMPARE(symInfo.label(0,0), QString());
    QCOMPARE(symInfo.contains(0), false);
    QCOMPARE(symInfo.isScalar(), false);
    QVERIFY(symInfo.lastOffset()<0);
    QCOMPARE(symInfo.firstSection(), -1);
    QVERIFY(symInfo.lastSection()<0);
    QVERIFY(symInfo.firstJaccSection()<0);
    QVERIFY(symInfo.lastJaccSection()<0);
    QCOMPARE(symInfo.labelTree(),  nullptr);
}

void TestSymbolInfo::test_getSet()
{
    int i = 0;
    const int value = 42;
    SymbolInfo symInfo;

    symInfo.setOffset(value+i);
    QCOMPARE(symInfo.offset(), value+i);

    ++i;
    symInfo.setEntries(value+i);
    QCOMPARE(symInfo.entries(), value+i);

    symInfo.setName("lala");
    QCOMPARE(symInfo.name(), "lala");

    ++i;
    symInfo.setDimension(value+i);
    QCOMPARE(symInfo.dimension(), value+i);

    ++i;
    symInfo.setType(value+i);
    QCOMPARE(symInfo.type(), value+i);

    ++i;
    symInfo.setFirstSection(value+i);
    QCOMPARE(symInfo.firstSection(), value+i);
    QCOMPARE(symInfo.lastSection(), value+i+symInfo.entries()-1);
    QCOMPARE(symInfo.firstJaccSection(), value+i-PredefinedHeaderLength);
    QCOMPARE(symInfo.lastJaccSection(), symInfo.lastSection()-PredefinedHeaderLength);
    QVERIFY(symInfo.contains(value+i));
    QVERIFY(symInfo.contains(symInfo.lastSection()));

    ++i;
    QStringList data { "a", "b" };
    symInfo.setLabels(value+i, data);
    QCOMPARE(symInfo.label(value+i, 0), "a");
    QCOMPARE(symInfo.label(value+i, 1), "b");
    QCOMPARE(symInfo.label(value+i, 2), QString());
    SectionLabels sectionLabels { { value+i, data } };
    QCOMPARE(symInfo.sectionLabels(), sectionLabels);

    QSharedPointer<LabelTreeItem> labelTree(new LabelTreeItem);
    symInfo.setLabelTree(labelTree);
    QVERIFY(symInfo.labelTree() == labelTree);
}

QTEST_APPLESS_MAIN(TestSymbolInfo)

#include "tst_testsymbolinfo.moc"
