#include <QtTest>

#include "aggregation.h"
#include "symbolinfo.h"

using namespace gams::studio::modelinspector;

Q_DECLARE_METATYPE(AggregationItem)
Q_DECLARE_METATYPE(SymbolInfo)
Q_DECLARE_METATYPE(LabelTreeItem*)

class TestAggregation : public QObject
{
    Q_OBJECT

public:
    TestAggregation();
    ~TestAggregation();

private slots:
    void initTestCase();

    void test_UelTreeItem_constructor();

    void test_UelTreeItem_clone_data();
    void test_UelTreeItem_clone();

    void test_UelTreeItem_sections_data();
    void test_UelTreeItem_sections();

    void test_UelTreeItem_sectionExtent_data();
    void test_UelTreeItem_sectionExtent();

    void test_AggregationItem_visibleSections_data();
    void test_AggregationItem_visibleSections();

    void test_Aggregator_aggregate_data();
    void test_Aggregator_aggregate();

private:
    void setTreeSections(LabelTreeItem *tree, int entries, int firstSection);

private:
    LabelTreeItem *mUelTree_0 = nullptr;
    LabelTreeItem *mUelTree_1 = nullptr;
    LabelTreeItem *mUelTree_2 = nullptr;
    LabelTreeItem *mUelTree_3 = nullptr;
    LabelTreeItem *mUelTree_4 = nullptr;
};

TestAggregation::TestAggregation()
{

}

TestAggregation::~TestAggregation()
{
    delete mUelTree_0;
    delete mUelTree_1;
    delete mUelTree_2;
    delete mUelTree_3;
    delete mUelTree_4;
}

void TestAggregation::initTestCase()
{
    mUelTree_0 = new LabelTreeItem;

    mUelTree_1 = new LabelTreeItem;
    auto uel_sea_0 = new LabelTreeItem("one", mUelTree_1);
    mUelTree_1->append(uel_sea_0);
    setTreeSections(mUelTree_1, 1, 0);

    mUelTree_2 = new LabelTreeItem;
    mUelTree_2->append(new LabelTreeItem("sea", mUelTree_2));
    mUelTree_2->append(new LabelTreeItem("san", mUelTree_2));
    setTreeSections(mUelTree_2, 2, 0);

    mUelTree_3 = new LabelTreeItem("x");
    auto uel_sea_1 = new LabelTreeItem("sea", mUelTree_3);
    mUelTree_3->append(uel_sea_1);
    uel_sea_1->append(new LabelTreeItem("N", uel_sea_1));
    uel_sea_1->append(new LabelTreeItem("C", uel_sea_1));
    uel_sea_1->append(new LabelTreeItem("T", uel_sea_1));
    auto uel_san_1 = new LabelTreeItem("san", mUelTree_3);
    mUelTree_3->append(uel_san_1);
    uel_san_1->append(new LabelTreeItem("N", uel_san_1));
    uel_san_1->append(new LabelTreeItem("C", uel_san_1));
    uel_san_1->append(new LabelTreeItem("T", uel_san_1));
    setTreeSections(mUelTree_3, 6, 5);

    mUelTree_4 = new LabelTreeItem("x");
    auto uel_sea_2 = new LabelTreeItem("sea", mUelTree_4);
    mUelTree_4->append(uel_sea_2);
    auto n_sea_1 = new LabelTreeItem("N", uel_sea_2);
    uel_sea_2->append(n_sea_1);
    n_sea_1->append(new LabelTreeItem("a", n_sea_1));
    n_sea_1->append(new LabelTreeItem("b", n_sea_1));
    auto c_sea_1 = new LabelTreeItem("C", uel_sea_2);
    uel_sea_2->append(c_sea_1);
    c_sea_1->append(new LabelTreeItem("a", c_sea_1));
    c_sea_1->append(new LabelTreeItem("b", c_sea_1));
    auto t_sea_1 = new LabelTreeItem("T", uel_sea_2);
    uel_sea_2->append(t_sea_1);
    t_sea_1->append(new LabelTreeItem("a", t_sea_1));
    t_sea_1->append(new LabelTreeItem("b", t_sea_1));
    auto uel_san_2 = new LabelTreeItem("san", mUelTree_4);
    mUelTree_4->append(uel_san_2);
    auto n_san_1 = new LabelTreeItem("N", uel_san_2);
    uel_san_2->append(n_san_1);
    n_san_1->append(new LabelTreeItem("a", n_san_1));
    n_san_1->append(new LabelTreeItem("b", n_san_1));
    auto c_san_1 = new LabelTreeItem("C", uel_san_2);
    uel_san_2->append(c_san_1);
    c_san_1->append(new LabelTreeItem("a", c_san_1));
    c_san_1->append(new LabelTreeItem("b", c_san_1));
    auto t_san_1 = new LabelTreeItem("T", uel_san_2);
    uel_san_2->append(t_san_1);
    t_san_1->append(new LabelTreeItem("a", t_san_1));
    t_san_1->append(new LabelTreeItem("b", t_san_1));
    setTreeSections(mUelTree_4, 12, 0);
}

void TestAggregation::test_UelTreeItem_constructor()
{

}

void TestAggregation::test_UelTreeItem_clone_data()
{
    QTest::addColumn<LabelTreeItem*>("item");
    QTest::addColumn<int>("expected");

    QTest::newRow("emty tree")         << mUelTree_0 <<  0;
    QTest::newRow("lv1 with 1 items")  << mUelTree_1 <<  1;
    QTest::newRow("lv1 with 2 items")  << mUelTree_2 <<  2;
    QTest::newRow("lv2 with 6 items")  << mUelTree_3 <<  6;
    QTest::newRow("lv2 with 12 items") << mUelTree_4 << 12;
}

void TestAggregation::test_UelTreeItem_clone()
{
    QFETCH(LabelTreeItem*, item);
    QFETCH(int, expected);

    auto clone = item->clone();
    int actual = clone->sectionExtent();
    QCOMPARE(actual, expected);
}

void TestAggregation::test_UelTreeItem_sections_data()
{
    QTest::addColumn<LabelTreeItem*>("item");
    QTest::addColumn<QSet<int>>("expected");

    QTest::addRow("empty tree") << mUelTree_0 << QSet<int>();
    QTest::addRow("1 levels, 1 sections") << mUelTree_1 << QSet<int> { 0 };
    QTest::addRow("1 levels, 2 sections") << mUelTree_2 << QSet<int> { 0, 1 };
    QTest::addRow("2 levels, 6 sections") << mUelTree_3 << QSet<int> { 5, 6, 7, 8, 9, 10 };
    QTest::addRow("3 levels, 12 sections") << mUelTree_4 << QSet<int> { 0, 1, 2, 3, 4, 5, 6 , 7, 8, 9, 10, 11 };
}

void TestAggregation::test_UelTreeItem_sections()
{
    QFETCH(LabelTreeItem*, item);
    QFETCH(QSet<int>, expected);

    QSet<int> actual = item->sections();
    QCOMPARE(actual, expected);
}

void TestAggregation::test_UelTreeItem_sectionExtent_data()
{
    QTest::addColumn<LabelTreeItem*>("item");
    QTest::addColumn<int>("expected");

    QTest::newRow("emty tree")         << mUelTree_0 <<  0;
    QTest::newRow("lv1 with 1 items")  << mUelTree_1 <<  1;
    QTest::newRow("lv1 with 2 items")  << mUelTree_2 <<  2;
    QTest::newRow("lv2 with 6 items")  << mUelTree_3 <<  6;
    QTest::newRow("lv2 with 12 items") << mUelTree_4 << 12;
}

void TestAggregation::test_UelTreeItem_sectionExtent()
{
    QFETCH(LabelTreeItem*, item);
    QFETCH(int, expected);

    int actual = item->sectionExtent();
    QCOMPARE(actual, expected);
}

void TestAggregation::test_AggregationItem_visibleSections_data()
{
    //
    //SymbolInfo symbol_2;
    //symbol_2.setDimension(3);
    //symbol_2.setEntries(12);
    //QSharedPointer<UelTreeItem> uelTree_2(new UelTreeItem("x"));
    //
    //QTest::newRow("no aggregation (3 dim)") << symbol_2 << AggregationItem() << 12;
    //
    //AggregationItem item_4;
    //item_4.setCheckState(1, Qt::Unchecked);
    //item_4.setCheckState(2, Qt::Unchecked);
    //item_4.setCheckState(3, Qt::Checked);
    //QTest::newRow("third dim") << symbol_2 << item_4 << 6;
    //
    //AggregationItem item_5;
    //item_5.setCheckState(1, Qt::Checked);
    //item_5.setCheckState(2, Qt::Checked);
    //item_5.setCheckState(3, Qt::Unchecked);
    //QTest::newRow("first two dims") << symbol_2 << item_5 << 2;
    //
    //AggregationItem item_6;
    //item_6.setCheckState(1, Qt::Unchecked);
    //item_6.setCheckState(2, Qt::Checked);
    //item_6.setCheckState(3, Qt::Checked);
    //QTest::newRow("last two dim") << symbol_2 << item_6 << 2;
    //
    //AggregationItem item_7;
    //item_7.setCheckState(1, Qt::Checked);
    //item_7.setCheckState(2, Qt::Unchecked);
    //item_7.setCheckState(3, Qt::Checked);
    //QTest::newRow("first and last dim") << symbol_2 << item_7 << 3;
    //
    //AggregationItem item_8;
    //item_8.setCheckState(1, Qt::Unchecked);
    //item_8.setCheckState(2, Qt::Checked);
    //item_8.setCheckState(3, Qt::Unchecked);
    //QTest::newRow("middle dim") << symbol_2 << item_8 << 4;
    //
    // //TODO tests with middle dim if sym.dim == 4 ???
    //
    // //TODO width test sea, san, ore
    //
    // //TODO test dim==0
}

void TestAggregation::test_AggregationItem_visibleSections()
{

}

void TestAggregation::test_Aggregator_aggregate_data()
{
    QTest::addColumn<SymbolInfo>("symbol");
    QTest::addColumn<AggregationItem>("item");
    QTest::addColumn<int>("expected");
    // TODO sections()

    QTest::newRow("invalid symbol") << SymbolInfo() << AggregationItem() << 0;

    SymbolInfo symbol_1;
    symbol_1.setDimension(2);
    symbol_1.setEntries(6);
    QSharedPointer<LabelTreeItem> uelTree_1(mUelTree_3->clone());
    symbol_1.setLabelTree(uelTree_1);

    QTest::newRow("empty aggregation") << symbol_1 << AggregationItem() << 6;

    //AggregationItem item_0;
    //item_0.setCheckState(1, Qt::Unchecked);
    //item_0.setCheckState(2, Qt::Unchecked);
    //QTest::newRow("all unchecked") << symbol_1 << item_0 << 6;
    //
    //AggregationItem item_1;
    //item_1.setCheckState(1, Qt::Checked);
    //item_1.setCheckState(2, Qt::Checked);
    //QTest::newRow("all checked") << symbol_1 << item_1 << 1;
    //
    //AggregationItem item_2;
    //item_2.setCheckState(1, Qt::Checked);
    //item_2.setCheckState(2, Qt::Unchecked);
    //QTest::newRow("first dim") << symbol_1 << item_2 << 3;
    //
    //AggregationItem item_3;
    //item_3.setCheckState(1, Qt::Unchecked);
    //item_3.setCheckState(2, Qt::Checked);
    //QTest::newRow("second dim") << symbol_1 << item_3 << 2;
}

void TestAggregation::test_Aggregator_aggregate()
{// TODO why does the test crash Qt Creator???
    //QFETCH(SymbolInfo, symbol);
    //QFETCH(AggregationItem, item);
    //QFETCH(int, expected);

    //auto newTree = Aggregator::aggregate(symbol, item);
    //int actualExtent = newTree->sectionExtent();
    //QCOMPARE(actualExtent, expected);
    //auto actualSections
    //QCOMPARE(actualSections, );
}

void TestAggregation::setTreeSections(LabelTreeItem *tree, int entries, int firstSection)
{
    QList<LabelTreeItem*> items { tree->childs() };
    while (!items.isEmpty()) {
        QList<LabelTreeItem*> nextLevel;
        for (int nodes=items.size(), gap=entries/nodes, currentSection=firstSection;
             nodes>0; --nodes, currentSection+=gap) {
            auto item = items.takeFirst();
            item->setSectionIndex(currentSection);
            if (!item->hasChildren())
                item->setSections({currentSection});
            nextLevel.append(item->childs());
        }
        items = nextLevel;
    }
}

QTEST_APPLESS_MAIN(TestAggregation)

#include "tst_testaggregation.moc"
