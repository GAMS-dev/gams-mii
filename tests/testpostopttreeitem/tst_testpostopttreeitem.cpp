#include <QtTest>

#include "postopttreeitem.h"

using namespace gams::studio::modelinspector;

class TestPostoptTreeItem : public QObject
{
    Q_OBJECT

public:
    TestPostoptTreeItem();
    ~TestPostoptTreeItem();

private slots:
    void test_GroupPostoptTreeItem_default();
    void test_GroupPostoptTreeItem_get_set();

    void test_LinePostoptTreeItem_default();
    void test_LinePostoptTreeItem_get_set();
};

TestPostoptTreeItem::TestPostoptTreeItem()
{

}

TestPostoptTreeItem::~TestPostoptTreeItem()
{

}

void TestPostoptTreeItem::test_GroupPostoptTreeItem_default()
{
    GroupPostoptTreeItem item;
    QCOMPARE(item.data(0).toString(), QString());
    QCOMPARE(item.data(1), QVariant());
    QCOMPARE(item.child(-8), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.rowCount(), 0);
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.parent(), nullptr);
}

void TestPostoptTreeItem::test_GroupPostoptTreeItem_get_set()
{
    GroupPostoptTreeItem* root = new GroupPostoptTreeItem("G1");
    QCOMPARE(root->data(0).toString(), "G1");
    QCOMPARE(root->data(1), QVariant());
    GroupPostoptTreeItem* item1 = new GroupPostoptTreeItem("I1", root);
    root->append(item1);
    QCOMPARE(root->rowCount(), 1);
    QCOMPARE(root->child(-1), nullptr);
    QCOMPARE(root->child(0), item1);
    QCOMPARE(root->child(1), nullptr);
    QCOMPARE(item1->data(0).toString(), "I1");
    QCOMPARE(item1->data(1), QVariant());
    QCOMPARE(item1->row(), 0);
    QCOMPARE(item1->parent(), root);
    item1->setParent(nullptr);
    QCOMPARE(item1->parent(), nullptr);
    item1->setParent(root);
    GroupPostoptTreeItem* item2 = new GroupPostoptTreeItem("I2", root);
    root->append(item2);
    QCOMPARE(item2->row(), 1);
    delete root;
}

void TestPostoptTreeItem::test_LinePostoptTreeItem_default()
{
    LinePostoptTreeItem item;
    QCOMPARE(item.data(0), QVariant());
    QCOMPARE(item.data(1), QVariant());
    QCOMPARE(item.child(-8), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.rowCount(), 0);
    QCOMPARE(item.columnCount(), 0);
    QCOMPARE(item.parent(), nullptr);
}

void TestPostoptTreeItem::test_LinePostoptTreeItem_get_set()
{
    LinePostoptTreeItem* root = new LinePostoptTreeItem({"G1"});
    QCOMPARE(root->data(0).toString(), "G1");
    QCOMPARE(root->data(1), QVariant());
    LinePostoptTreeItem* item1 = new LinePostoptTreeItem({"I1", "E"}, root);
    root->append(item1);
    QCOMPARE(root->rowCount(), 1);
    QCOMPARE(root->child(-1), nullptr);
    QCOMPARE(root->child(0), item1);
    QCOMPARE(root->child(1), nullptr);
    QCOMPARE(item1->data(0).toString(), "I1");
    QCOMPARE(item1->data(1), "E");
    QCOMPARE(item1->data(2), QVariant());
    QCOMPARE(item1->row(), 0);
    QCOMPARE(item1->parent(), root);
    item1->setParent(nullptr);
    QCOMPARE(item1->parent(), nullptr);
    item1->setParent(root);
    LinePostoptTreeItem* item2 = new LinePostoptTreeItem({"I2", "X"}, root);
    root->append(item2);
    QCOMPARE(item2->row(), 1);
    delete root;
}

QTEST_APPLESS_MAIN(TestPostoptTreeItem)

#include "tst_testpostopttreeitem.moc"
