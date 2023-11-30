#include <QtTest>

#include "datamatrix.h"

using namespace gams::studio::mii;

class TestDataMatrix : public QObject
{
    Q_OBJECT

private slots:
    void test_DataRow();
    void test_DataRow_inputValue();
    void test_DataRow_outputValue();

    void test_DataMatrix_defaults();
    void test_DataMatrix();
};

void TestDataMatrix::test_DataRow()
{
    DataRow dataRow0;
    QCOMPARE(dataRow0.entries(), 0);
    QCOMPARE(dataRow0.entriesNl(), 0);
    QCOMPARE(dataRow0.colIdx(), nullptr);
    QCOMPARE(dataRow0.inputData(), nullptr);
    QCOMPARE(dataRow0.nlFlags(), nullptr);
    DataRow dataRow1(8);
    dataRow1.setEntriesNl(2);
    QCOMPARE(dataRow1.entries(), 8);
    QCOMPARE(dataRow1.entriesNl(), 2);
    QVERIFY(dataRow1.colIdx() != nullptr);
    QVERIFY(dataRow1.inputData() != nullptr);
    QVERIFY(dataRow1.nlFlags() != nullptr);
    std::fill(dataRow1.colIdx(), dataRow1.colIdx()+dataRow1.entries(), 1);
    std::fill(dataRow1.inputData(), dataRow1.inputData()+dataRow1.entries(), 0);
    std::fill(dataRow1.nlFlags(), dataRow1.nlFlags()+dataRow1.entries(), 0);
    DataRow dataRow2(dataRow1);
    QCOMPARE(dataRow2.entries(), dataRow1.entries());
    QCOMPARE(dataRow2.entriesNl(), 2);
    QVERIFY(std::equal(dataRow1.colIdx(), dataRow1.colIdx()+dataRow1.entries(), dataRow2.colIdx()));
    QVERIFY(std::equal(dataRow1.inputData(), dataRow1.inputData()+dataRow1.entries(), dataRow2.inputData()));
    QVERIFY(std::equal(dataRow1.nlFlags(), dataRow1.nlFlags()+dataRow1.entries(), dataRow2.nlFlags()));
    DataRow dataRow3;
    dataRow3 = dataRow2;
    QCOMPARE(dataRow3.entries(), dataRow2.entries());
    QCOMPARE(dataRow3.entriesNl(), 2);
    QVERIFY(std::equal(dataRow2.colIdx(), dataRow2.colIdx()+dataRow2.entries(), dataRow3.colIdx()));
    QVERIFY(std::equal(dataRow2.inputData(), dataRow2.inputData()+dataRow2.entries(), dataRow3.inputData()));
    QVERIFY(std::equal(dataRow2.nlFlags(), dataRow2.nlFlags()+dataRow2.entries(), dataRow3.nlFlags()));
    DataRow dataRow4 = std::move(dataRow3);
    //QCOMPARE(dataRow3.entries(), 0);
    //QCOMPARE(dataRow3.colIdx(), nullptr);
    //QCOMPARE(dataRow3.inputData(), nullptr);
    //QCOMPARE(dataRow3.nlFlags(), nullptr);
    QCOMPARE(dataRow4.entries(), dataRow2.entries());
    QCOMPARE(dataRow4.entriesNl(), 2);
    QVERIFY(std::equal(dataRow2.colIdx(), dataRow2.colIdx()+dataRow2.entries(), dataRow4.colIdx()));
    QVERIFY(std::equal(dataRow2.inputData(), dataRow2.inputData()+dataRow2.entries(), dataRow4.inputData()));
    QVERIFY(std::equal(dataRow2.nlFlags(), dataRow2.nlFlags()+dataRow2.entries(), dataRow4.nlFlags()));
    DataRow dataRow5(DataRow(12));
    QCOMPARE(dataRow5.entries(), 12);
    QCOMPARE(dataRow5.entriesNl(), 0);
    QVERIFY(dataRow5.colIdx() != nullptr);
    QVERIFY(dataRow5.inputData() != nullptr);
    QVERIFY(dataRow5.nlFlags() != nullptr);
}

void TestDataMatrix::test_DataRow_inputValue()
{
    DataRow dataRow0;
    QCOMPARE(dataRow0.inputValue(-1, 10), QVariant());
    QCOMPARE(dataRow0.inputValue(0, 10), QVariant());
    QCOMPARE(dataRow0.inputValue(4, 1), QVariant());
    DataRow dataRow1(4);
    dataRow1.inputData()[0] = 0;
    dataRow1.inputData()[1] = 1;
    dataRow1.inputData()[2] = 2;
    dataRow1.inputData()[3] = 3;
    dataRow1.colIdx()[0] = 0;
    dataRow1.colIdx()[1] = 1;
    dataRow1.colIdx()[2] = 2;
    dataRow1.colIdx()[3] = 3;
    QCOMPARE(dataRow1.inputValue(-1, -4), QVariant());
    QCOMPARE(dataRow1.inputValue(0, 1).toDouble(), dataRow1.inputData()[0]);
    QCOMPARE(dataRow1.inputValue(1, 4).toDouble(), dataRow1.inputData()[1]);
    QCOMPARE(dataRow1.inputValue(2, 5).toDouble(), dataRow1.inputData()[2]);
    QCOMPARE(dataRow1.inputValue(3, 4).toDouble(), dataRow1.inputData()[3]);
    QCOMPARE(dataRow1.inputValue(3, -1), QVariant());
}

void TestDataMatrix::test_DataRow_outputValue()
{
    DataRow dataRow0;
    QCOMPARE(dataRow0.outputValue(-1, 10), QVariant());
    QCOMPARE(dataRow0.outputValue(0, 10), QVariant());
    QCOMPARE(dataRow0.outputValue(4, 1), QVariant());
    DataRow dataRow1(4);
    dataRow1.outputData()[0] = 0;
    dataRow1.outputData()[1] = 1;
    dataRow1.outputData()[2] = 2;
    dataRow1.outputData()[3] = 3;
    dataRow1.colIdx()[0] = 0;
    dataRow1.colIdx()[1] = 1;
    dataRow1.colIdx()[2] = 2;
    dataRow1.colIdx()[3] = 3;
    QCOMPARE(dataRow1.outputValue(-1, -4), QVariant());
    QCOMPARE(dataRow1.outputValue(0, 1).toDouble(), dataRow1.outputData()[0]);
    QCOMPARE(dataRow1.outputValue(1, 4).toDouble(), dataRow1.outputData()[1]);
    QCOMPARE(dataRow1.outputValue(2, 5).toDouble(), dataRow1.outputData()[2]);
    QCOMPARE(dataRow1.outputValue(3, 4).toDouble(), dataRow1.outputData()[3]);
    QCOMPARE(dataRow1.outputValue(3, -1), QVariant());
}

void TestDataMatrix::test_DataMatrix_defaults()
{
    DataMatrix dataMatrix;
    QCOMPARE(dataMatrix.rowCount(), 0);
    QCOMPARE(dataMatrix.row(-1), nullptr);
    QCOMPARE(dataMatrix.row(0), nullptr);
    QCOMPARE(dataMatrix.row(1), nullptr);
    QCOMPARE(dataMatrix.isLinear(), true);
    QCOMPARE(dataMatrix.columnCount(), 0);
    QCOMPARE(dataMatrix.evalPoint(), nullptr);
}

void TestDataMatrix::test_DataMatrix()
{
    DataMatrix dataMatrix0;
    QCOMPARE(dataMatrix0.rowCount(), 0);
    QCOMPARE(dataMatrix0.row(-1), nullptr);
    QCOMPARE(dataMatrix0.row(0), nullptr);
    QCOMPARE(dataMatrix0.row(1), nullptr);
    QCOMPARE(dataMatrix0.columnCount(), 0);
    QCOMPARE(dataMatrix0.evalPoint(), nullptr);
    DataMatrix dataMatrix1(8, 16, 0);
    QCOMPARE(dataMatrix1.rowCount(), 8);
    QCOMPARE(dataMatrix1.row(-1), nullptr);
    QVERIFY(dataMatrix1.row(0) != nullptr);
    QVERIFY(dataMatrix1.row(8) != nullptr);
    QCOMPARE(dataMatrix1.row(9), nullptr);
    QCOMPARE(dataMatrix1.columnCount(), 16);
    QVERIFY(dataMatrix1.evalPoint() != nullptr);
    DataMatrix dataMatrix2(dataMatrix1);
    QCOMPARE(dataMatrix2.rowCount(), 8);
    QCOMPARE(dataMatrix2.row(-1), nullptr);
    QVERIFY(dataMatrix2.row(0) != nullptr);
    QVERIFY(dataMatrix2.row(8) != nullptr);
    QCOMPARE(dataMatrix2.row(9), nullptr);
    QCOMPARE(dataMatrix2.columnCount(), 16);
    QVERIFY(dataMatrix2.evalPoint() != nullptr);
    DataMatrix dataMatrix3 = dataMatrix2;
    QCOMPARE(dataMatrix3.rowCount(), dataMatrix2.rowCount());
    QCOMPARE(dataMatrix3.columnCount(), dataMatrix2.columnCount());
    DataMatrix dataMatrix4 = std::move(dataMatrix3);
    //QCOMPARE(dataMatrix3.rowCount(), 0);
    //QCOMPARE(dataMatrix3.row(0), nullptr);
    QCOMPARE(dataMatrix4.rowCount(), 8);
    QVERIFY(dataMatrix4.row(8) != nullptr);
    QCOMPARE(dataMatrix4.columnCount(), 16);
    QVERIFY(dataMatrix4.evalPoint() != nullptr);
    DataMatrix dataMatrix5(DataMatrix(12, 8, 0));
    QCOMPARE(dataMatrix5.rowCount(), 12);
    QVERIFY(dataMatrix5.row(0) != nullptr);
    QVERIFY(dataMatrix5.row(12) != nullptr);
    QVERIFY(dataMatrix5.row(13) == nullptr);
    QCOMPARE(dataMatrix5.columnCount(), 8);
    QVERIFY(dataMatrix5.evalPoint() != nullptr);
}

QTEST_APPLESS_MAIN(TestDataMatrix)

#include "tst_testdatamatrix.moc"
