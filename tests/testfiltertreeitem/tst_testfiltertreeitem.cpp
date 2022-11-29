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
    void test_static();
    void test_default();
    void test_constructor();
    void test_getSet();
    void test_getSetChilds();
    void test_subTreeCheckState();
    void test_isEnabled();
};

TestFilterTreeItem::TestFilterTreeItem()
{

}

TestFilterTreeItem::~TestFilterTreeItem()
{

}

void TestFilterTreeItem::test_static()
{
    QCOMPARE(FilterTreeItem::EquationText, "Equations");
    QCOMPARE(FilterTreeItem::VariableText, "Variables");
}

void TestFilterTreeItem::test_default()
{
    FilterTreeItem item;
    QCOMPARE(item.hasChildren(), false);
    QVERIFY(item.child(0) == nullptr);
    QCOMPARE(item.childs(), QList<FilterTreeItem*>());
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.rowCount(), 0);
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.parent(), nullptr);
    QCOMPARE(item.text(), QString());
    QCOMPARE(item.isCheckable(), true);
    QCOMPARE(item.isEnabled(), true);
    QCOMPARE(item.checked(), Qt::Checked);
    QCOMPARE(item.sectionIndex(), -1);
    QCOMPARE(item.symbolIndex(), -1);
    QVERIFY(item.isVisible());
}

void TestFilterTreeItem::test_constructor()
{
    FilterTreeItem item("x", Qt::Unchecked, nullptr);
    QCOMPARE(item.text(), "x");
    QCOMPARE(item.checked(), Qt::Unchecked);
    QCOMPARE(item.parent(), nullptr);
}

void TestFilterTreeItem::test_getSet()
{
    FilterTreeItem item;

    auto parent = new FilterTreeItem;
    item.setParent(parent);
    QCOMPARE(item.parent(), parent);
    item.setParent(nullptr);
    delete parent;

    item.setText("demand");
    QCOMPARE(item.text(), "demand");

    item.setCheckable(false);
    QCOMPARE(item.isCheckable(), false);

    item.setEnabled(false);
    QCOMPARE(item.isEnabled(), false);

    item.setChecked(Qt::PartiallyChecked);
    QCOMPARE(item.checked(), Qt::PartiallyChecked);

    item.setSectionIndex(42);
    QCOMPARE(item.sectionIndex(), 42);

    item.setSymbolIndex(84);
    QCOMPARE(item.symbolIndex(), 84);

    item.setVisible(false);
    QVERIFY(!item.isVisible());
}

void TestFilterTreeItem::test_getSetChilds()
{
    FilterTreeItem item_1;
    QCOMPARE(item_1.child(-1), nullptr);
    QCOMPARE(item_1.child(0), nullptr);
    QCOMPARE(item_1.child(1), nullptr);

    FilterTreeItem item_2;
    auto c1 = new FilterTreeItem("c1", Qt::Checked, &item_2);
    item_2.append(c1);
    QCOMPARE(item_2.child(-1), nullptr);
    QCOMPARE(item_2.child(0), c1);
    QCOMPARE(item_2.child(1), nullptr);

    auto c2 = new FilterTreeItem("c2", Qt::Checked, &item_2);
    item_2.append(c2);
    auto childs = item_2.childs();
    QCOMPARE(childs.size(), 2);
    QCOMPARE(childs.at(0)->text(), "c1");
    QCOMPARE(childs.at(1)->text(), "c2");
}

void TestFilterTreeItem::test_subTreeCheckState()
{
    FilterTreeItem root;
    auto c1 = new FilterTreeItem("c1", Qt::Checked, &root);
    root.append(c1);
    auto c2 = new FilterTreeItem("c2", Qt::Checked, &root);
    root.append(c2);

    root.setSubTreeState(Qt::Unchecked);
    QCOMPARE(root.checked(), Qt::Unchecked);

    root.setSubTreeState(Qt::Checked);
    QCOMPARE(root.checked(), Qt::Checked);

    c2->setSubTreeState(Qt::Unchecked);
    QCOMPARE(root.checked(), Qt::Checked);

    c2->setChecked(Qt::Unchecked);
    QCOMPARE(root.checked(), Qt::PartiallyChecked);
}

void TestFilterTreeItem::test_isEnabled()
{
    FilterTreeItem root;
    auto c1 = new FilterTreeItem("c1", Qt::Checked, &root);
    root.append(c1);
    auto c2 = new FilterTreeItem("c2", Qt::Checked, &root);
    root.append(c2);
    QVERIFY(root.isEnabled());

    c2->setEnabled(false);
    QVERIFY(root.isEnabled());

    c1->setEnabled(false);
    QVERIFY(!root.isEnabled());

    auto c3 = new FilterTreeItem("c3", Qt::Checked, &root);
    c2->append(c3);
    QVERIFY(root.isEnabled());

    c3->setEnabled(false);
    QVERIFY(!root.isEnabled());
}

QTEST_APPLESS_MAIN(TestFilterTreeItem)

#include "tst_testfiltertreeitem.moc"
