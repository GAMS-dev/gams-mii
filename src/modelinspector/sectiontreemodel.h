#ifndef SECTIONTREEMODEL_H
#define SECTIONTREEMODEL_H

#include <QAbstractItemModel>

namespace gams {
namespace studio {
namespace modelinspector {

class TreeItem
{// TODO (AF) move to dedicated file?
public:
    // TODO (AF) Additional colums needed? If not get rid of QVector -> so far no
    explicit TreeItem(const QVector<QVariant> &data,
                      int page,
                      TreeItem *parent = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);
    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;

    int page() const {
        return mPage;
    }

    void setPage(int page) {
        mPage = page;
    }

    int row() const;
    TreeItem *parent();

private:
    TreeItem *mParent;
    QVector<TreeItem*> mChilds;
    QVector<QVariant> mData;
    int mPage;
};

class SectionTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SectionTreeModel(QObject *parent = nullptr);
    ~SectionTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, // TODO needed?
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void loadModelData(const QStringList &data);

private:
    TreeItem *mRoot;
};

}
}
}

#endif // SECTIONTREEMODEL_H
