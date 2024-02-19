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

#include "abstractviewframe.h"
#include "sectiontreeitem.h"

using namespace gams::studio::mii;

Q_DECLARE_METATYPE(SectionGroupTreeItem*)
Q_DECLARE_METATYPE(SectionTreeItem*)

class TestSectionTreeItem : public QObject
{
    Q_OBJECT

private slots:
    void test_setTypeText();

    void test_SectionGroupTreeItem_default();
    void test_SectionGroupTreeItem_constructor();
    void test_SectionGroupTreeItem_getSet();
    void test_SectionGroupTreeItem_removeAllChilds();
    void test_SectionGroupTreeItem_isActive();
    void test_SectionGroupTreeItem_setActive();
    void test_SectionGroupTreeItem_isGroup();

    void test_SectionTreeItem_default();
    void test_SectionTreeItem_constructor();
    void test_SectionTreeItem_getSet();
    void test_SectionTreeItem_removeAllChilds();
    void test_SectionTreeItem_isActive();
    void test_SectionTreeItem_setActive();
    void test_SectionTreeItem_isGroup();
};

void TestSectionTreeItem::test_setTypeText()
{
    SectionGroupTreeItem item("default");
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
    QCOMPARE(item.type(), ViewHelper::ViewDataType::BlockpicGroup);
    item.setType("lala");
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_default()
{
    SectionGroupTreeItem item("default");
    item.remove(4, 2);
    QCOMPARE(item.child(-1), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childs(), QList<AbstractSectionTreeItem*>());
    QCOMPARE(item.childCount(), 0);
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.text(), "default");
    QCOMPARE(item.widget(), nullptr);
    QCOMPARE(item.widgets(), QList<AbstractViewFrame*>());
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    QCOMPARE(item.parent(), nullptr);
    QCOMPARE(item.isCustom(), false);
    QCOMPARE(item.isGroup(), false);
    QCOMPARE(item.removeChilds(), QList<AbstractViewFrame*>());
}

void TestSectionTreeItem::test_SectionGroupTreeItem_constructor()
{
    SectionGroupTreeItem item1("default");
    QCOMPARE(item1.text(), "default");
    QCOMPARE(item1.parent(), nullptr);
    QCOMPARE(item1.isGroup(), false);
    SectionGroupTreeItem item2("other", &item1);
    QCOMPARE(item2.text(), "other");
    QCOMPARE(item2.parent(), &item1);
    QCOMPARE(item2.widget(), nullptr);
    QCOMPARE(item2.isGroup(), false);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_getSet()
{
    SectionGroupTreeItem item("default");

    auto c1 = new SectionGroupTreeItem("c1", &item);
    item.append(c1);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childCount(), 1);
    auto c2 = new SectionGroupTreeItem("c2", &item);
    item.append(c2);
    QCOMPARE(item.child(0), c1);
    QCOMPARE(item.child(1), c2);
    QCOMPARE(item.child(2), nullptr);
    QCOMPARE(item.childCount(), 2);

    item.setText("some entry");
    QCOMPARE(item.text(), "some entry");

    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    item.setType(ViewHelper::ViewDataType::Symbols);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Symbols);

    item.setCustom(true);
    QCOMPARE(item.isCustom(), true);

    QCOMPARE(item.isGroup(), false);

    item.setParent(c1);
    QCOMPARE(item.parent(), c1);
    item.setParent(nullptr);
    QCOMPARE(item.parent(), nullptr);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_removeAllChilds()
{
    SectionGroupTreeItem item("default");
    QCOMPARE(item.childCount(), 0);
    item.removeAllChilds();
    QCOMPARE(item.childCount(), 0);
    auto c1 = new SectionGroupTreeItem("c1", &item);
    item.append(c1);
    auto c2 = new SectionGroupTreeItem("c2", &item);
    item.append(c2);
    QCOMPARE(item.childCount(), 2);
    item.removeAllChilds();
    QCOMPARE(item.childCount(), 0);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_isActive()
{
    SectionGroupTreeItem item("default");
    QCOMPARE(item.isActive(), false);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_setActive()
{
    SectionGroupTreeItem item("default", nullptr);
    QCOMPARE(item.isActive(), false);
    item.setActive(true);
    QCOMPARE(item.isActive(), false);
}

void TestSectionTreeItem::test_SectionGroupTreeItem_isGroup()
{
    SectionGroupTreeItem item("default", nullptr);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Overview);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Count);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Average);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Scaling);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::Postopt);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::Symbols);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BlockpicGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::SymbolsGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::CustomGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::PredefinedGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::ModelInstanceGroup);
    QCOMPARE(item.isGroup(), true);
}

void TestSectionTreeItem::test_SectionTreeItem_default()
{
    SectionTreeItem item("default", nullptr);
    item.remove(4, 2);
    QCOMPARE(item.child(-1), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.child(1), nullptr);
    QCOMPARE(item.childs(), QList<AbstractSectionTreeItem*>());
    QCOMPARE(item.childCount(), 0);
    QCOMPARE(item.columnCount(), 1);
    QCOMPARE(item.text(), "default");
    QCOMPARE(item.widget(), nullptr);
    QCOMPARE(item.widgets(), QList<AbstractViewFrame*>());
    QCOMPARE(item.row(), 0);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    QCOMPARE(item.parent(), nullptr);
    QCOMPARE(item.isCustom(), false);
    QCOMPARE(item.isGroup(), false);
    QCOMPARE(item.removeChilds(), QList<AbstractViewFrame*>());
}

void TestSectionTreeItem::test_SectionTreeItem_constructor()
{
    SectionTreeItem item1("default", nullptr);
    QCOMPARE(item1.text(), "default");
    QCOMPARE(item1.parent(), nullptr);
    QCOMPARE(item1.widget(), nullptr);
    QCOMPARE(item1.isGroup(), false);
    SectionTreeItem item2("other", nullptr, &item1);
    QCOMPARE(item2.text(), "other");
    QCOMPARE(item2.parent(), &item1);
    QCOMPARE(item2.widget(), nullptr);
    QCOMPARE(item2.isGroup(), false);
}

void TestSectionTreeItem::test_SectionTreeItem_getSet()
{
    SectionTreeItem item("default", nullptr);

    auto c1 = new SectionTreeItem("c1", nullptr, &item);
    item.append(c1);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.childCount(), 0);

    auto c2 = new SectionTreeItem("c2", nullptr, &item);
    item.append(c2);
    QCOMPARE(item.child(0), nullptr);
    QCOMPARE(item.childCount(), 0);

    item.setText("some entry");
    QCOMPARE(item.text(), "some entry");

    QCOMPARE(item.type(), ViewHelper::ViewDataType::Unknown);
    item.setType(ViewHelper::ViewDataType::Symbols);
    QCOMPARE(item.type(), ViewHelper::ViewDataType::Symbols);

    item.setCustom(true);
    QCOMPARE(item.isCustom(), true);

    QCOMPARE(item.isGroup(), false);

    item.setParent(c1);
    QCOMPARE(item.parent(), c1);
    item.setParent(nullptr);
    QCOMPARE(item.parent(), nullptr);

    delete c1;
    delete c2;
}

void TestSectionTreeItem::test_SectionTreeItem_removeAllChilds()
{
    SectionTreeItem item("default", nullptr);
    QCOMPARE(item.childCount(), 0);
    item.removeAllChilds();
    QCOMPARE(item.childCount(), 0);
    auto c1 = new SectionTreeItem("c1", nullptr, &item);
    item.append(c1);
    QCOMPARE(item.childCount(), 0);
    item.removeAllChilds();
    QCOMPARE(item.childCount(), 0);
    delete c1;
}

void TestSectionTreeItem::test_SectionTreeItem_isActive()
{
    SectionTreeItem item("default", nullptr);
    QCOMPARE(item.isActive(), false);
}

void TestSectionTreeItem::test_SectionTreeItem_setActive()
{
    SectionTreeItem item("default", nullptr);
    QCOMPARE(item.isActive(), false);
    item.setActive(true);
    QCOMPARE(item.isActive(), true);
}

void TestSectionTreeItem::test_SectionTreeItem_isGroup()
{
    SectionTreeItem item("default", nullptr);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Overview);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Count);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Average);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BP_Scaling);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::Postopt);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::Symbols);
    QCOMPARE(item.isGroup(), false);
    item.setType(ViewHelper::ViewDataType::BlockpicGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::SymbolsGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::CustomGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::PredefinedGroup);
    QCOMPARE(item.isGroup(), true);
    item.setType(ViewHelper::ViewDataType::ModelInstanceGroup);
    QCOMPARE(item.isGroup(), true);
}

QTEST_APPLESS_MAIN(TestSectionTreeItem)

#include "tst_testsectiontreeitem.moc"
