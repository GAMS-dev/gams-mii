#ifndef BLOCKPICTREEMODEL_H
#define BLOCKPICTREEMODEL_H

#include <QAbstractItemModel>

namespace gams {
namespace studio{
namespace modelinspector {

class ModelInstance;
class BlockpicTreeItem;

class BlockpicTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit BlockpicTreeModel(BlockpicTreeItem *rootItem,
                               QObject *parent = nullptr);
    ~BlockpicTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    BlockpicTreeItem *mRootItem;
};

}
}
}

#endif // BLOCKPICTREEMODEL_H
