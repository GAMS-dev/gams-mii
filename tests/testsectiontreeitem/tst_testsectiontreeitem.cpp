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

#include "sectiontreeitem.h"
#include "common.h"

using namespace gams::studio::mii;

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
    QCOMPARE(item.widget(), nullptr);
    QCOMPARE(item.widgets(), QList<AbstractViewFrame*>());
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    QCOMPARE(item.parent(), nullptr);
    QCOMPARE(item.isCustom(), false);
    QCOMPARE(item.isGroup(), false);
    QCOMPARE(item.removeChilds(), QList<AbstractViewFrame*>());
}

void TestSectionTreeItem::test_constructor()
{
    SectionTreeItem item1("default");
    QCOMPARE(item1.name(), "default");
    QCOMPARE(item1.parent(), nullptr);
    SectionTreeItem item2("other", nullptr, &item1);
    QCOMPARE(item2.name(), "other");
    QCOMPARE(item2.widget(), nullptr);
    QCOMPARE(item2.parent(), &item1);
}

void TestSectionTreeItem::test_getSet()
{
    SectionTreeItem item("default");

    auto c1 = new SectionTreeItem("c1", &item);
    item.append(c1);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childCount(), 1);
    auto c2 = new SectionTreeItem("c2", &item);
    item.append(c2);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), c2);
    QCOMPARE(item.child(2), nullptr);
    QCOMPARE(item.childCount(), 2);

    item.setName("some entry");
    QCOMPARE(item.name(), "some entry");

    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    item.setType(ViewHelper::ViewDataType::Symbols);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Symbols);

    // TODO tests: Add an empty frame for tests?
    //auto frame = new PostoptTreeViewFrame;
    //item.setWidget(frame);
    //QCOMPARE(item.widget(), frame);
    //item.setWidget(nullptr);
    //QCOMPARE(item.widget(), nullptr);
    //delete frame;

    item.setCustom(true);
    QCOMPARE(item.isCustom(), true);

    item.setGroup(true);
    QCOMPARE(item.isGroup(), true);

    item.setParent(c1);
    QCOMPARE(item.parent(), c1);
    item.setParent(nullptr);
    QCOMPARE(item.parent(), nullptr);
}

void TestSectionTreeItem::test_setTypeText()
{
    SectionTreeItem item("default");
    item.setType(ViewHelper::BPScaling);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::BP_Scaling);
    item.setType(ViewHelper::BPOverview);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::BP_Overview);
    item.setType(ViewHelper::BPCount);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::BP_Count);
    item.setType(ViewHelper::BPAverage);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::BP_Average);
    item.setType(ViewHelper::Postopt);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Postopt);
    item.setType(ViewHelper::SymbolView);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Symbols);
    item.setType(ViewHelper::Blockpic);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Blockpic);
    item.setType("lala");
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
}

QTEST_APPLESS_MAIN(TestSectionTreeItem)

#include "tst_testsectiontreeitem.moc"
