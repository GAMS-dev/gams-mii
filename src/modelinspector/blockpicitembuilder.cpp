#include "blockpicitembuilder.h"
#include "blockpictreeitem.h"
#include "modelinstance.h"

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

BlockpicItemBuilder::BlockpicItemBuilder(ModelInstance *modelInstance)
    : mModelInstance(modelInstance)
{

}

BlockpicItemBuilder::~BlockpicItemBuilder() {

}




DetailLevelOneBuilder::DetailLevelOneBuilder(ModelInstance *modelInstance)
    : BlockpicItemBuilder(modelInstance)
{

}

BlockpicTreeItem* DetailLevelOneBuilder::dataStructure()
{
    auto* rootItem = new BlockpicRootItem;
    rootItem->setData(headerData());
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dcteqnSymType)) {
        buildSymbolRows(rootItem, symbol);
    }
    auto* item = new BlockpicEqnItem(rootItem);
    QVector<QVariant> data { QStringLiteral("Variable Type") };
    data.append(mModelInstance->variableType());
    data.append(QVector<QVariant>(rootItem->columns()-data.size(), ""));
    item->setData(data);
    rootItem->append(item);
    return rootItem;
}

QVector<QVariant> DetailLevelOneBuilder::headerData()
{
    QVector<QVariant> entries { QStringLiteral("Equation") };
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dctvarSymType)) {
        entries.append(symbol.Name);
    }
    entries.append(QStringLiteral("Type"));
    entries.append(QStringLiteral("RHS"));
    return entries;
}

void DetailLevelOneBuilder::buildSymbolRows(BlockpicTreeItem *rootItem,
                                            const SymbolInfo &symbol)
{
    auto* item = new BlockpicEqnItem(rootItem);
    QVector<QVariant> data { symbol.Name };
    data.append(mModelInstance->variableData(symbol));
    data.append(QVector<QVariant>(rootItem->columns()-data.size()-2, ""));
    data.append({mModelInstance->equationType(symbol.Offset),
                 mModelInstance->aggregatedRhs(symbol)});
    item->setData(data);
    rootItem->append(item);
}




DetailLevelTwoBuilder::DetailLevelTwoBuilder(ModelInstance *modelInstance)
    : BlockpicItemBuilder(modelInstance)
{

}

BlockpicTreeItem* DetailLevelTwoBuilder::dataStructure()
{
    auto* rootItem = new BlockpicRootItem;
    rootItem->setData(headerData());
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dcteqnSymType)) {
        buildSymbolRows(rootItem, symbol);
    }
    buildTotalVarRow(rootItem);
    return rootItem;
}

QVector<QVariant> DetailLevelTwoBuilder::headerData()
{
    QVector<QVariant> entries { QStringLiteral("Equation") };
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dctvarSymType)) {
        entries.append(symbol.Name);
    }
    entries.append(QStringLiteral("RHS"));
    entries.append(QStringLiteral("Eqn"));
    return entries;
}

void DetailLevelTwoBuilder::buildSymbolRows(BlockpicTreeItem *rootItem,
                                            const SymbolInfo &symbol)
{
    auto* item = new BlockpicEqnItem(rootItem);
    QVector<QVariant> data { symbol.Name };
    data.append(QVector<QVariant>(rootItem->columns()-data.size(), ""));
    item->setData(data);
    rootItem->append(item);
    buildMaxMinRows(item, symbol);
}

void DetailLevelTwoBuilder::buildMaxMinRows(BlockpicTreeItem *eqnItem,
                                            const SymbolInfo &symbol)
{
    auto scaling = mModelInstance->equationVariableScaling(symbol);
    auto eqnScaling = mModelInstance->equationScaling(symbol);
    auto maxminRhs = mModelInstance->maxminRhs(symbol.Offset, symbol.Entries);
    for (int i=0; i<2; ++i) {
        auto* item = new BlockpicUelItem(eqnItem);
        QVector<QVariant> data { i ? "Min" : "Max" };
        Q_FOREACH(const auto& maxmin, scaling) {
            if (maxmin.Valid)
                data.append(i ? maxmin.Min : maxmin.Max);
            else
                data.append("");
        }
        data.append(QVector<QVariant>(eqnItem->columns()-data.size()-2, ""));
        if (symbol.isScalar()) // TODO rules to have empty RHS row, on 0 only?
            data.append(QString());
        else
            data.append(i ? maxminRhs.second : maxminRhs.first);
        data.append(i ? eqnScaling.Min : eqnScaling.Max);
        item->setData(data);
        eqnItem->append(item);
    }
}

void DetailLevelTwoBuilder::buildTotalVarRow(BlockpicTreeItem *rootItem)
{
    auto *totalItem = new BlockpicEqnItem(rootItem);
    QVector<QVariant> data { QStringLiteral("Total Var") };
    data.append(QVector<QVariant>(rootItem->columns()-data.size(), ""));
    totalItem->setData(data);
    rootItem->append(totalItem);

    auto scaling = mModelInstance->totalScaling();
    auto rhs = mModelInstance->totalRhs();
    for (int i=0; i<2; ++i) {
        auto* item = new BlockpicUelItem(totalItem);
        QVector<QVariant> data { i ? "Min" : "Max" };
        Q_FOREACH(const auto& maxmin, scaling) {
            data.append(i ? maxmin.Min : maxmin.Max);
        }
        data.append(QVector<QVariant>(totalItem->columns()-data.size()-2, ""));
        data.append(i ? rhs.second : rhs.first);
        data.append(QString());
        item->setData(data);
        totalItem->append(item);
    }
}




DetailLevelThreeBuilder::DetailLevelThreeBuilder(ModelInstance *modelInstance)
    : BlockpicItemBuilder(modelInstance)
{

}

BlockpicTreeItem* DetailLevelThreeBuilder::dataStructure()
{
    auto* rootItem = new BlockpicRootItem;
    rootItem->setData(headerData());
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dcteqnSymType)) {
        buildSymbolRows(rootItem, symbol);
    }
    return rootItem;
}

QVector<QVariant> DetailLevelThreeBuilder::headerData()
{
    QVector<QVariant> entries { QStringLiteral("Equation") };
    Q_FOREACH(const auto& symbol, mModelInstance->symbols(dctvarSymType)) {
        if (symbol.Entries == 1) {
            entries.append(symbol.Name);
        } else if (symbol.Entries > 1) {
            auto data = mModelInstance->columnUelStrings(symbol);
            entries.append(data);
        }
    }
    entries.append(QStringLiteral("Type"));
    entries.append(QStringLiteral("RHS"));
    return entries;
}

void DetailLevelThreeBuilder::buildSymbolRows(BlockpicTreeItem *rootItem,
                                              const SymbolInfo &symbol)
{
    auto* item = new BlockpicEqnItem(rootItem);
    QVector<QVariant> data { symbol.Name };
    if (mModelInstance->rowUels(symbol).isEmpty()) {
        data.append(mModelInstance->scalarEquationData(symbol.Offset));
        data.append({mModelInstance->equationType(symbol.Offset),
                     mModelInstance->rhs(symbol.Offset)});
    } else {
        data.append(QVector<QVariant>(rootItem->columns()-data.size(), ""));
    }
    item->setData(data);
    rootItem->append(item);

    buildUelRows(item, symbol);
}

void DetailLevelThreeBuilder::buildUelRows(BlockpicTreeItem *eqnItem,
                                           const SymbolInfo &symbol)
{
    auto uels = mModelInstance->rowUels(symbol);
    for (int i=0; i<uels.size(); ++i) {
        int rowIndex = mModelInstance->rowIndex(symbol.Offset+i);
        if (rowIndex < 0)
            continue;

        auto * item = new BlockpicUelItem(eqnItem);

        QVector<QVariant> data { uels.at(i) };
        data.append(QVector<QVariant>(eqnItem->columns()-data.size()-2, ""));
        auto colData = mModelInstance->equationData(rowIndex);
        for (auto iter=colData.constKeyValueBegin(); iter!=colData.constKeyValueEnd(); ++iter) {
            data[iter->first+1] = iter->second;
        }
        data.append({ mModelInstance->equationType(symbol.Offset + i),
                      mModelInstance->rhs(symbol.Offset + i) });
        item->setData(data);
        eqnItem->append(item);
    }
}

}
}
}
