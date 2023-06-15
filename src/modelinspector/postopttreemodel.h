#ifndef POSTOPTTREEMODEL_H
#define POSTOPTTREEMODEL_H

#include <QAbstractItemModel>

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;
class PostoptTreeItem;

class PostoptTreeModel final : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PostoptTreeModel(int view,
                              QSharedPointer<AbstractModelInstance> modelInstance,
                              QObject *parent = nullptr);

    ~PostoptTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QSharedPointer<PostoptTreeItem> mRootItem;
    int mView;
};

}
}
}

#endif // POSTOPTTREEMODEL_H
