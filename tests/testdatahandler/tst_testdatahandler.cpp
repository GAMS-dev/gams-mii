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

#include "datahandler.h"
#include "abstractmodelinstance.h"

using namespace gams::studio::mii;

class TestDataHandler : public QObject
{
    Q_OBJECT

public:
    TestDataHandler();
    ~TestDataHandler();

private slots:
    void test_DataRow();
    void test_DataRow_value();

    void test_DataMatrix();
    void test_DataHandler_empty();
};

TestDataHandler::TestDataHandler()
{

}

TestDataHandler::~TestDataHandler()
{

}

void TestDataHandler::test_DataRow()
{
    DataRow dataRow0;
    QCOMPARE(dataRow0.entries(), 0);
    QCOMPARE(dataRow0.colIdx(), nullptr);
    QCOMPARE(dataRow0.data(), nullptr);
    DataRow dataRow1(8);
    QCOMPARE(dataRow1.entries(), 8);
    QVERIFY(dataRow1.colIdx() != nullptr);
    QVERIFY(dataRow1.data() != nullptr);
    std::fill(dataRow1.colIdx(), dataRow1.colIdx()+dataRow1.entries(), 1);
    std::fill(dataRow1.data(), dataRow1.data()+dataRow1.entries(), 0);
    DataRow dataRow2(dataRow1);
    QCOMPARE(dataRow2.entries(), dataRow1.entries());
    QVERIFY(std::equal(dataRow1.colIdx(), dataRow1.colIdx()+dataRow1.entries(), dataRow2.colIdx()));
    QVERIFY(std::equal(dataRow1.data(), dataRow1.data()+dataRow1.entries(), dataRow2.data()));
    DataRow dataRow3;
    dataRow3 = dataRow2;
    QCOMPARE(dataRow3.entries(), dataRow2.entries());
    QVERIFY(std::equal(dataRow2.colIdx(), dataRow2.colIdx()+dataRow2.entries(), dataRow3.colIdx()));
    QVERIFY(std::equal(dataRow2.data(), dataRow2.data()+dataRow2.entries(), dataRow3.data()));
    DataRow dataRow4 = std::move(dataRow3);
    QCOMPARE(dataRow3.entries(), 0);
    QCOMPARE(dataRow3.colIdx(), nullptr);
    QCOMPARE(dataRow3.data(), nullptr);
    QCOMPARE(dataRow4.entries(), dataRow2.entries());
    QVERIFY(std::equal(dataRow2.colIdx(), dataRow2.colIdx()+dataRow2.entries(), dataRow4.colIdx()));
    QVERIFY(std::equal(dataRow2.data(), dataRow2.data()+dataRow2.entries(), dataRow4.data()));
    DataRow dataRow5(DataRow(12));
    QCOMPARE(dataRow5.entries(), 12);
    QVERIFY(dataRow5.colIdx() != nullptr);
    QVERIFY(dataRow5.data() != nullptr);
}

void TestDataHandler::test_DataRow_value()
{
    DataRow dataRow0;
    QCOMPARE(dataRow0.value(-1, 10), QVariant());
    QCOMPARE(dataRow0.value(0, 10), QVariant());
    QCOMPARE(dataRow0.value(4, 1), QVariant());
    DataRow dataRow1(4);
    dataRow1.data()[0] = 0;
    dataRow1.data()[1] = 1;
    dataRow1.data()[2] = 2;
    dataRow1.data()[3] = 3;
    dataRow1.colIdx()[0] = 0;
    dataRow1.colIdx()[1] = 1;
    dataRow1.colIdx()[2] = 2;
    dataRow1.colIdx()[3] = 3;
    QCOMPARE(dataRow1.value(-1, -4), QVariant());
    QCOMPARE(dataRow1.value(0, 1).toDouble(), dataRow1.data()[0]);
    QCOMPARE(dataRow1.value(1, 4).toDouble(), dataRow1.data()[1]);
    QCOMPARE(dataRow1.value(2, 5).toDouble(), dataRow1.data()[2]);
    QCOMPARE(dataRow1.value(3, 4).toDouble(), dataRow1.data()[3]);
    QCOMPARE(dataRow1.value(3, -1), QVariant());
}

void TestDataHandler::test_DataMatrix()
{
    DataMatrix dataMatrix0;
    QCOMPARE(dataMatrix0.rowCount(), 0);
    QCOMPARE(dataMatrix0.row(-1), nullptr);
    QCOMPARE(dataMatrix0.row(0), nullptr);
    QCOMPARE(dataMatrix0.row(1), nullptr);
    DataMatrix dataMatrix1(8);
    QCOMPARE(dataMatrix1.rowCount(), 8);
    QCOMPARE(dataMatrix1.row(-1), nullptr);
    QVERIFY(dataMatrix1.row(0) != nullptr);
    QVERIFY(dataMatrix1.row(8) != nullptr);
    QCOMPARE(dataMatrix1.row(9), nullptr);
    DataMatrix dataMatrix2(dataMatrix1);
    QCOMPARE(dataMatrix2.rowCount(), 8);
    QCOMPARE(dataMatrix2.row(-1), nullptr);
    QVERIFY(dataMatrix2.row(0) != nullptr);
    QVERIFY(dataMatrix2.row(8) != nullptr);
    QCOMPARE(dataMatrix2.row(9), nullptr);
    DataMatrix dataMatrix3 = dataMatrix2;
    QCOMPARE(dataMatrix3.rowCount(), dataMatrix2.rowCount());
    DataMatrix dataMatrix4 = std::move(dataMatrix3);
    QCOMPARE(dataMatrix3.rowCount(), 0);
    QCOMPARE(dataMatrix3.row(0), nullptr);
    QCOMPARE(dataMatrix4.rowCount(), 8);
    QVERIFY(dataMatrix4.row(8) != nullptr);
    DataMatrix dataMatrix5(DataMatrix(12));
    QCOMPARE(dataMatrix5.rowCount(), 12);
    QVERIFY(dataMatrix5.row(0) != nullptr);
    QVERIFY(dataMatrix5.row(12) != nullptr);
    QVERIFY(dataMatrix5.row(13) == nullptr);
}

void TestDataHandler::test_DataHandler_empty()
{
    EmptyModelInstance modelInstance;
    DataHandler dataHandler(modelInstance);
    dataHandler.aggregate(nullptr);
    QCOMPARE(dataHandler.data(-4, -2, -1), QVariant());
    QCOMPARE(dataHandler.data(4, 2, -1), QVariant());
    QCOMPARE(dataHandler.data(4, 2, 0), QVariant());
    QCOMPARE(dataHandler.data(4, 2, 84), QVariant());
    QCOMPARE(dataHandler.headerData(-1, Qt::Horizontal, -1), -1);
    QCOMPARE(dataHandler.headerData(-1, Qt::Vertical, -1), -1);
    QCOMPARE(dataHandler.headerData(1, Qt::Horizontal, 1), -1);
    QCOMPARE(dataHandler.headerData(1, Qt::Vertical, 1), -1);
    QCOMPARE(dataHandler.rowCount(-1), 0);
    QCOMPARE(dataHandler.rowCount(0), 0);
    QCOMPARE(dataHandler.rowEntries(-1, -1), 0);
    QCOMPARE(dataHandler.rowEntries(0, 0), 0);
    QCOMPARE(dataHandler.columnCount(-1), 0);
    QCOMPARE(dataHandler.columnCount(0), 0);
    QCOMPARE(dataHandler.columnEntries(-1, -1), 0);
    QCOMPARE(dataHandler.columnEntries(0, 0), 0);
    QCOMPARE(dataHandler.maxSymbolDimension(0, Qt::Horizontal), 0);
    QCOMPARE(dataHandler.maxSymbolDimension(0, Qt::Vertical), 0);
    dataHandler.loadJacobian();
}

QTEST_APPLESS_MAIN(TestDataHandler)

#include "tst_testdatahandler.moc"
