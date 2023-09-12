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
#include "abstractmodelinstance.h"
#include "postopttreeitem.h"

#include <QDir>

namespace gams {
namespace studio {
namespace mii {

AbstractModelInstance::AbstractModelInstance(const QString &workspace,
                                             const QString &systemDir,
                                             const QString &scratchDir)
    : mScratchDir(scratchDir)
    , mWorkspace(QDir(workspace).absolutePath())
    , mSystemDir(systemDir)
{

}

AbstractModelInstance::~AbstractModelInstance()
{

}

QString AbstractModelInstance::workspace() const
{
    return mWorkspace;
}

void AbstractModelInstance::setWorkspace(const QString &workspace)
{
    mWorkspace = QDir(workspace).absolutePath();
}

QString AbstractModelInstance::systemDirectory() const
{
    return mSystemDir;
}

void AbstractModelInstance::setSystemDirectory(const QString &systemDir)
{
    mSystemDir = systemDir;
}

QString AbstractModelInstance::scratchDirectory() const
{
    return mScratchDir;
}

void AbstractModelInstance::setScratchDirectory(const QString &scratchDir)
{
    mScratchDir = scratchDir;
}

bool AbstractModelInstance::useOutput() const
{
    return mUseOutput;
}

void AbstractModelInstance::setUseOutput(bool useOutput)
{
    mUseOutput = useOutput;
}

QString AbstractModelInstance::logMessages() {
    auto messages = mLogMessages.join("\n");
    mLogMessages.clear();
    return messages;
}

QString AbstractModelInstance::modelName() const
{
    return QString();
}

int AbstractModelInstance::equationCount() const
{
    return 0;
}

int AbstractModelInstance::equationCount(EquationType type) const
{
    Q_UNUSED(type);
    return 0;
}

int AbstractModelInstance::equationRowCount() const
{
    return 0;
}

Symbol* AbstractModelInstance::equation(int sectionIndex) const
{
    Q_UNUSED(sectionIndex);
    return nullptr;
}

int AbstractModelInstance::variableCount() const
{
    return 0;
}

int AbstractModelInstance::variableCount(VariableType type) const
{
    Q_UNUSED(type);
    return 0;
}

int AbstractModelInstance::variableRowCount() const
{
    return 0;
}

Symbol* AbstractModelInstance::variable(int sectionIndex) const
{
    Q_UNUSED(sectionIndex);
    return nullptr;
}

const QStringList &AbstractModelInstance::labels() const
{
    return mLabels;
}

QVariant AbstractModelInstance::equationAttribute(const QString &header,
                                                  int index,
                                                  int entry,
                                                  bool abs) const
{
    Q_UNUSED(header);
    Q_UNUSED(index);
    Q_UNUSED(entry);
    Q_UNUSED(abs);
    return QVariant();
}

QVariant AbstractModelInstance::variableAttribute(const QString &header,
                                                  int index,
                                                  int entry,
                                                  bool abs) const
{
    Q_UNUSED(header);
    Q_UNUSED(index);
    Q_UNUSED(entry);
    Q_UNUSED(abs);
    return QVariant();
}

AbstractModelInstance::State AbstractModelInstance::state() const
{
    return mState;
}

EmptyModelInstance::EmptyModelInstance(const QString &workspace,
                                       const QString &systemDir,
                                       const QString &scratchDir)
    : AbstractModelInstance(workspace, systemDir, scratchDir)
    , mRootItem(new LinePostoptTreeItem)
{

}

EmptyModelInstance::~EmptyModelInstance()
{

}

const QVector<Symbol*> &EmptyModelInstance::equations() const
{
    return mSymbols;
}

const QVector<Symbol*> &EmptyModelInstance::variables() const
{
    return mSymbols;
}

void EmptyModelInstance::variableLowerBounds(double *bounds)
{
    Q_UNUSED(bounds);
}

void EmptyModelInstance::variableUpperBounds(double *bounds)
{
    Q_UNUSED(bounds);
}

double EmptyModelInstance::rhs(int row) const
{
    Q_UNUSED(row);
    return 0.0;
}

QString EmptyModelInstance::longestEquationText() const
{
    return QString();
}

QString EmptyModelInstance::longestVariableText() const
{
    return QString();
}

int EmptyModelInstance::maximumEquationDimension() const
{
    return 0;
}

int EmptyModelInstance::maximumVariableDimension() const
{
    return 0;
}

double EmptyModelInstance::modelMinimum() const
{
    return std::numeric_limits<double>::lowest();
}

double EmptyModelInstance::modelMaximum() const
{
    return std::numeric_limits<double>::max();
}

const QVector<Symbol*> &EmptyModelInstance::symbols(Symbol::Type type) const
{
    Q_UNUSED(type);
    return mSymbols;
}

void EmptyModelInstance::loadData()
{

}

void EmptyModelInstance::loadJacobian()
{

}

char EmptyModelInstance::equationType(int row) const
{
    Q_UNUSED(row);
    return 0;
}

char EmptyModelInstance::variableType(int column) const
{
    Q_UNUSED(column);
    return 0;
}

int EmptyModelInstance::rowCount(int view) const
{
    Q_UNUSED(view);
    return 0;
}

int EmptyModelInstance::rowEntries(int row, int view) const
{
    Q_UNUSED(row);
    Q_UNUSED(view);
    return 0;
}

int EmptyModelInstance::columnCount(int view) const
{
    Q_UNUSED(view);
    return 0;
}

int EmptyModelInstance::columnEntries(int column, int view) const
{
    Q_UNUSED(column);
    Q_UNUSED(view);
    return 0;
}

int EmptyModelInstance::symbolRowCount(int view) const
{
    Q_UNUSED(view);
    return 0;
}

int EmptyModelInstance::symbolColumnCount(int view) const
{
    Q_UNUSED(view);
    return 0;
}

QSharedPointer<AbstractViewConfiguration> EmptyModelInstance::clone(int view, int newView)
{
    Q_UNUSED(view);
    Q_UNUSED(newView);
    return nullptr;
}

void EmptyModelInstance::loadData(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    Q_UNUSED(viewConfig);
}

QVariant EmptyModelInstance::data(int row, int column, int view) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(view);
    return QVariant();
}

QSharedPointer<PostoptTreeItem> EmptyModelInstance::dataTree(int view) const
{
    Q_UNUSED(view);
    return mRootItem;
}


QVariant EmptyModelInstance::headerData(int logicalIndex,
                                        Qt::Orientation orientation,
                                        int view, int role) const
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(orientation);
    Q_UNUSED(view);
    Q_UNUSED(role);
    return QVariant();
}

QVariant EmptyModelInstance::plainHeaderData(Qt::Orientation orientation,
                                             int view, int logicalIndex,
                                             int dimension) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(view);
    Q_UNUSED(logicalIndex);
    Q_UNUSED(dimension);
    return QVariant();
}

void EmptyModelInstance::jacobianData(DataMatrix &dataMatrix)
{
    Q_UNUSED(dataMatrix);
}

}
}
}
