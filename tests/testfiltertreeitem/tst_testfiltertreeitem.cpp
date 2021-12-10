#include <QtTest>

#include "filtertreeitem.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(FilterTreeItem)
Q_DECLARE_METATYPE(FilterTreeItem*)

class TestFilterTreeItem : public QObject
{
    Q_OBJECT

public:
    TestFilterTreeItem();
    ~TestFilterTreeItem();

private slots:
    void test_blank();
    void test_getSet();

    void test_appendAndChild();

    void test_branches();
    void test_siblings();

    //void test_checked();
    //void test_sectionExtent();
};

TestFilterTreeItem::TestFilterTreeItem()
{

}

TestFilterTreeItem::~TestFilterTreeItem()
{

}

void TestFilterTreeItem::test_blank()
{
    FilterTreeItem item;
    QCOMPARE(item.hasChildren(), false);
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.rowCount(), 0);
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.parent(), nullptr);
    QCOMPARE(item.text(), QString());
    QCOMPARE(item.isCheckable(), true);
    QCOMPARE(item.isEnabled(), true);
    QCOMPARE(item.checked(), Qt::Checked);
    QCOMPARE(item.index(), -1);
}

void TestFilterTreeItem::test_getSet()
{

}

void TestFilterTreeItem::test_appendAndChild()
{
    FilterTreeItem item_1;
    QCOMPARE(item_1.child(-1), nullptr);
    QCOMPARE(item_1.child(0), nullptr);
    QCOMPARE(item_1.child(1), nullptr);

    FilterTreeItem item_2;
    auto c1 = new FilterTreeItem("c1", false, &item_2);
    item_2.append(c1);
    QCOMPARE(item_2.child(-1), nullptr);
    QCOMPARE(item_2.child(0), c1);
    QCOMPARE(item_2.child(1), nullptr);
}

void TestFilterTreeItem::test_branches()
{

}

void TestFilterTreeItem::test_siblings()
{

}


QTEST_APPLESS_MAIN(TestFilterTreeItem)

#include "tst_testfiltertreeitem.moc"
