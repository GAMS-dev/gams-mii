#include <QtTest>

#include "symbolinfo.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(SymbolInfo)


class TestSymbolInfo : public QObject
{
    Q_OBJECT

public:
    TestSymbolInfo();
    ~TestSymbolInfo();

private slots:
    void test_blank();
    void test_getSet();
};

TestSymbolInfo::TestSymbolInfo()
{

}

TestSymbolInfo::~TestSymbolInfo()
{

}

void TestSymbolInfo::test_blank()
{
    SymbolInfo symInfo;
    QCOMPARE(symInfo.offset(), -1);
    QCOMPARE(symInfo.entries(), -1);
    QCOMPARE(symInfo.name(), QString());
    QCOMPARE(symInfo.dimension(), -1);
    QCOMPARE(symInfo.type(), -1);
    QMap<int, QStringList> uels;
    QCOMPARE(symInfo.sectionLabels(), uels);
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

    ++i;
    QStringList data { "a", "b" };
    symInfo.setLabels(value+i, data);
    QCOMPARE(symInfo.label(value+i, 0), "a");
    QCOMPARE(symInfo.label(value+i, 1), "b");
    QCOMPARE(symInfo.label(value+i, 2), QString());
}

QTEST_APPLESS_MAIN(TestSymbolInfo)

#include "tst_testsymbolinfo.moc"
