#include <QtTest>

#include "labeltreeitem.h"

using namespace gams::studio::modelinspector;

class TestLabelTreeItem : public QObject
{// TODO tests
    Q_OBJECT

public:
    TestLabelTreeItem();
    ~TestLabelTreeItem();

private slots:
    void test_constructor();
    void test_default();
    void test_getSet();
    void test_visibleChilds();
    void test_clone();
    void test_firstSectionIndex();
    void test_siblings();
    void test_sections();
    void test_unitedSections();
    void test_visibleSections();
    void test_visibleSectionsSorted();
    void test_sectionLabels();
    void test_isVisible();
    void test_sectionExtent();
    void test_unite();
};

TestLabelTreeItem::TestLabelTreeItem()
{

}

TestLabelTreeItem::~TestLabelTreeItem()
{

}

void TestLabelTreeItem::test_constructor()
{
    LabelTreeItem defaultItem;
    LabelTreeItem item("lala", &defaultItem);
    QCOMPARE(item.text(), "lala");
    QCOMPARE(item.parent(), &defaultItem);
}

void TestLabelTreeItem::test_default()
{
    LabelTreeItem item;
    QCOMPARE(item.child(-1), nullptr);
    QCOMPARE(item.child(0), nullptr);
    QVERIFY(item.childs().isEmpty());
    QVERIFY(item.visibleChilds().isEmpty());
    auto clone = item.clone();
    QVERIFY(clone != nullptr);
    delete clone;
    QCOMPARE(item.hasChildren(), false);
    QVERIFY(item.isRoot());
    QCOMPARE(item.firstSectionIndex(), -1);
    QCOMPARE(item.sectionIndex(), -1);
    QCOMPARE(item.parent(), nullptr);
    QVERIFY(item.siblings().isEmpty());
    QCOMPARE(item.size(), 0);
    QVERIFY(item.sections().isEmpty());
    QVERIFY(item.unitedSections().isEmpty());
    QVERIFY(item.visibleSections().isEmpty());
    QVERIFY(item.visibleSectionsSorted().isEmpty());
    QVERIFY(item.sectionLabels(-1, -1).isEmpty());
    QVERIFY(item.sectionLabels(0, 0).isEmpty());
    QVERIFY(item.sectionLabels(1, 1).isEmpty());
    QCOMPARE(item.text(), QString());
    QVERIFY(item.isVisible());
    QCOMPARE(item.sectionExtent(), 0);
}

void TestLabelTreeItem::test_getSet()
{
    LabelTreeItem item;

    auto child = new LabelTreeItem;
    item.append(child);
    QCOMPARE(item.child(0), child);
    QVERIFY(item.hasChildren());
    item.remove(child);
    QVERIFY(!item.hasChildren());
    item.setChilds({child});
    QVERIFY(item.hasChildren());
    item.setChilds({});
    QVERIFY(item.childs().isEmpty());
    delete child;

    item.setSectionIndex(42);
    QCOMPARE(item.sectionIndex(), 42);

    auto parent = new LabelTreeItem;
    item.setParent(parent);
    QCOMPARE(item.parent(), parent);
    item.setParent(nullptr);
    delete parent;

    QSet<int> sections { 1, 2, 3 };
    item.setSections(sections);
    QCOMPARE(item.sections(), sections);

    item.setText("some item");
    QCOMPARE(item.text(), "some item");

    item.setVisible(true);
    QVERIFY(item.isVisible());
}

void TestLabelTreeItem::test_visibleChilds()
{

}

void TestLabelTreeItem::test_clone()
{

}

void TestLabelTreeItem::test_firstSectionIndex()
{

}

void TestLabelTreeItem::test_siblings()
{

}

void TestLabelTreeItem::test_sections()
{

}

void TestLabelTreeItem::test_unitedSections()
{

}

void TestLabelTreeItem::test_visibleSections()
{

}

void TestLabelTreeItem::test_visibleSectionsSorted()
{

}

void TestLabelTreeItem::test_sectionLabels()
{

}

void TestLabelTreeItem::test_isVisible()
{

}

void TestLabelTreeItem::test_sectionExtent()
{

}

void TestLabelTreeItem::test_unite()
{

}

QTEST_APPLESS_MAIN(TestLabelTreeItem)

#include "tst_testlabeltreeitem.moc"
