/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023-2025 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023-2025 GAMS Development Corp. <support@gams.com>
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
 */
#include <QtTest>

#include "gamsprocess.h"

class TestGamsProcess : public QObject
{
    Q_OBJECT

public:
    TestGamsProcess();
    ~TestGamsProcess();

private slots:
    void init();
    void cleanup();

    void test_init();
    void test_getset_data();
    void test_getset();

private:
    GAMSProcess *mProc;
};

TestGamsProcess::TestGamsProcess()
{

}

TestGamsProcess::~TestGamsProcess()
{

}

void TestGamsProcess::init()
{
    mProc = new GAMSProcess(this);
}

void TestGamsProcess::cleanup()
{
    delete mProc;
    mProc = nullptr;
}

void TestGamsProcess::test_init()
{
    QCOMPARE(mProc->parent(), this);
    QCOMPARE(mProc->application(), "gams");
    QCOMPARE(mProc->directory(), QString());
    QCOMPARE(mProc->parameters(), QStringList());
    QCOMPARE(mProc->model(), QString());
}

void TestGamsProcess::test_getset_data()
{
    QTest::addColumn<QString>("dir");
    QTest::addColumn<QStringList>("params");
    QTest::addColumn<QString>("model");

    QTest::newRow("init vals") << QString() << QStringList() << QString();
    QTest::newRow("real vals") << "/some/dir" <<QStringList{"/??", "keep=1"} << "model.gms";
}

void TestGamsProcess::test_getset()
{
    QFETCH(QString, dir);
    QFETCH(QStringList, params);
    QFETCH(QString, model);

    mProc->setDirectory(dir);
    QCOMPARE(mProc->directory(), QDir::toNativeSeparators(dir));
    mProc->setParameters(params);
    QCOMPARE(mProc->parameters(), params);
    mProc->setModel(model);
    QCOMPARE(mProc->model(), model);
}

QTEST_APPLESS_MAIN(TestGamsProcess)

#include "tst_testgamsprocess.moc"
