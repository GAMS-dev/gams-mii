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

#include "modelinstance.h"

using namespace gams::studio::mii;

class TestModelInstance : public QObject
{
    Q_OBJECT

private slots:
    void test_constructor_initialize();
    void test_default();
    void test_getSet();
};

void TestModelInstance::test_constructor_initialize()
{
    const QString workspace = "myWorkDir";
    const QString systemDir = "mySysDir";
    const QString scratchDir = "myScratchDir";
    ModelInstance instance(false, workspace, systemDir, scratchDir);
    QVERIFY(!instance.logMessages().isEmpty());
    QCOMPARE(instance.state(), AbstractModelInstance::Error);
    auto realWorkspace = QDir(workspace).absolutePath();
    QCOMPARE(instance.workspace(), realWorkspace);
    QCOMPARE(instance.systemDirectory(), systemDir);
    QCOMPARE(instance.scratchDirectory(), scratchDir);
    QCOMPARE(instance.useOutput(), false);
    QCOMPARE(instance.globalAbsolute(), false);
    QCOMPARE(instance.maxSymbolDimension(0, Qt::Horizontal), 0);
    QCOMPARE(instance.maxSymbolDimension(0, Qt::Vertical), 0);
}

void TestModelInstance::test_default()
{
    ModelInstance instance;
    QVERIFY(!instance.logMessages().isEmpty());
    QCOMPARE(instance.state(), AbstractModelInstance::Error);
    QCOMPARE(instance.workspace(), QDir(".").absolutePath());
    QCOMPARE(instance.systemDirectory(), QString());
    QCOMPARE(instance.scratchDirectory(), QString());
    QCOMPARE(instance.useOutput(), false);
    // Needed because of the GAMS Windows registry magic and build machine setup
    QVERIFY(!instance.modelName().compare(QString()) ||
            !instance.modelName().compare("GAMS Model"));
    QCOMPARE(instance.globalAbsolute(), false);
    QCOMPARE(instance.maxSymbolDimension(0, Qt::Horizontal), 0);
    QCOMPARE(instance.maxSymbolDimension(0, Qt::Vertical), 0);
}

void TestModelInstance::test_getSet()
{
    ModelInstance instance;
    QVERIFY(!instance.logMessages().isEmpty());
    QCOMPARE(instance.state(), AbstractModelInstance::Error);
    instance.setWorkspace("my_workspace");
    QCOMPARE(instance.workspace(), QDir("my_workspace").absolutePath());
    instance.setSystemDirectory("my_system_dir");
    QCOMPARE(instance.systemDirectory(), "my_system_dir");
    instance.setScratchDirectory("my_scratch_dir");
    QCOMPARE(instance.scratchDirectory(), "my_scratch_dir");
    instance.setUseOutput(true);
    QCOMPARE(instance.useOutput(), true);
    instance.setGlobalAbsolute(true);
    QCOMPARE(instance.globalAbsolute(), true);
}

QTEST_APPLESS_MAIN(TestModelInstance)

#include "tst_testmodelinstance.moc"
