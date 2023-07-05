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
