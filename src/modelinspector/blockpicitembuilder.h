#ifndef BLOCKPICITEMBUILDER_H
#define BLOCKPICITEMBUILDER_H

#include <QVector>
#include <QVariant>

namespace gams {
namespace studio{
namespace modelinspector {

class BlockpicTreeItem;
class ModelInstance;
struct SymbolInfo;

// TODO
//  o class names

class BlockpicItemBuilder
{
protected:
    BlockpicItemBuilder(ModelInstance *modelInstance);

public:
    ~BlockpicItemBuilder();

    virtual BlockpicTreeItem* dataStructure() = 0;

protected:
    virtual QVector<QVariant> headerData() = 0;

    virtual void buildSymbolRows(BlockpicTreeItem *rootItem,
                                 const SymbolInfo &symbol) = 0;

protected:
    ModelInstance *mModelInstance;
};




class DetailLevelOneBuilder : public BlockpicItemBuilder
{
public:
    DetailLevelOneBuilder(ModelInstance *modelInstance);

    BlockpicTreeItem* dataStructure() override;

protected:
    QVector<QVariant> headerData() override;

    void buildSymbolRows(BlockpicTreeItem *rootItem,
                         const SymbolInfo &symbol) override;
};




class DetailLevelTwoBuilder : public BlockpicItemBuilder
{
public:
    DetailLevelTwoBuilder(ModelInstance *modelInstance);

    BlockpicTreeItem* dataStructure() override;

protected:
    QVector<QVariant> headerData() override;

    void buildSymbolRows(BlockpicTreeItem *rootItem,
                         const SymbolInfo &symbol) override;

private:
    void buildMaxMinRows(BlockpicTreeItem *eqnItem, const SymbolInfo &symbol);

    void buildTotalVarRow(BlockpicTreeItem *rootItem);
};




class DetailLevelThreeBuilder : public BlockpicItemBuilder
{
public:
    DetailLevelThreeBuilder(ModelInstance *modelInstance);

    BlockpicTreeItem* dataStructure() override;

protected:
    QVector<QVariant> headerData() override;

    void buildSymbolRows(BlockpicTreeItem *rootItem,
                         const SymbolInfo &symbol) override;

private:

    void buildUelRows(BlockpicTreeItem *eqnItem, const SymbolInfo &symbol);
};

}
}
}

#endif // BLOCKPICITEMBUILDER_H
