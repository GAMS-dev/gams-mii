#include <QtTest>

#include "sectiontreeitem.h"
#include "common.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(SectionTreeItem*);

class TestSectionTreeItem : public QObject
{
    Q_OBJECT

public:
    TestSectionTreeItem();
    ~TestSectionTreeItem();

private slots:
    void test_default();
    void test_constructor();
    void test_getSet();
    void test_setTypeText();
};

TestSectionTreeItem::TestSectionTreeItem()
{

}

TestSectionTreeItem::~TestSectionTreeItem()
{

}

void TestSectionTreeItem::test_default()
{
    SectionTreeItem item("default");
    item.remove(4, 2);
    QCOMPARE(item.child(-1), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childCount(), 0);
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.name(), "default");
    QCOMPARE(item.page(), -1);
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.type(), PredefinedViewEnum::Unknown);
    QCOMPARE(item.parent(), nullptr);
}

void TestSectionTreeItem::test_constructor()
{
    SectionTreeItem item1("default", nullptr);
    QCOMPARE(item1.name(), "default");
    QCOMPARE(item1.parent(), nullptr);
    SectionTreeItem item2("other", 42, &item1);
    QCOMPARE(item2.name(), "other");
    QCOMPARE(item2.page(), 42);
    QCOMPARE(item2.parent(), &item1);
}

void TestSectionTreeItem::test_getSet()
{
    SectionTreeItem item("default");

    auto c1 = new SectionTreeItem("c1", 0, &item);
    item.append(c1);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childCount(), 1);
    auto c2 = new SectionTreeItem("c2", 1, &item);
    item.append(c2);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), c2);
    QCOMPARE(item.child(2), nullptr);
    QCOMPARE(item.childCount(), 2);

    item.setName("some entry");
    QCOMPARE(item.name(), "some entry");

    item.setPage(42);
    QCOMPARE(item.page(), 42);

    item.setType(PredefinedViewEnum::Jaccobian);
    QCOMPARE(item.type(), PredefinedViewEnum::Jaccobian);
}

void TestSectionTreeItem::test_setTypeText()
{
    SectionTreeItem item("default");
    item.setType(constant->EquationAttributes);
    QCOMPARE(item.type(), PredefinedViewEnum::EqnAttributes);
    item.setType(constant->VariableAttributes);
    QCOMPARE(item.type(), PredefinedViewEnum::VarAttributes);
    item.setType(constant->Jaccobian);
    QCOMPARE(item.type(), PredefinedViewEnum::Jaccobian);
    item.setType(constant->FullView);
    QCOMPARE(item.type(), PredefinedViewEnum::Full);
    item.setType(constant->Statistic);
    QCOMPARE(item.type(), PredefinedViewEnum::Statistic);
    item.setType(constant->MinMax);
    QCOMPARE(item.type(), PredefinedViewEnum::MinMax);
    item.setType("lala");
    QCOMPARE(item.type(), PredefinedViewEnum::Unknown);
}

QTEST_APPLESS_MAIN(TestSectionTreeItem)

#include "tst_testsectiontreeitem.moc"
