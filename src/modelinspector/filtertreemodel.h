#ifndef FILTERTREEMODEL_H
#define FILTERTREEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace gams {
namespace studio {
namespace modelinspector {

class FilterTreeItem;

class AggregationTreeItemFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AggregationTreeItemFilterProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const override;
};

class FilterTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit FilterTreeModel(FilterTreeItem *rootItem, QObject *parent = nullptr);
    ~FilterTreeModel();

    FilterTreeItem* filterItem() const;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void updateParents(const QModelIndex &currentIndex,
                       const QVector<int> &roles = QVector<int>());
    void updateChilds(const QModelIndex &currentIndex,
                      const QVector<int> &roles = QVector<int>());

private:
    FilterTreeItem *mRootItem = nullptr;
};

}
}
}

#endif // FILTERTREEMODEL_H
