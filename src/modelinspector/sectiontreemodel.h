#ifndef SECTIONTREEMODEL_H
#define SECTIONTREEMODEL_H

#include <QAbstractItemModel>

namespace gams {
namespace studio {
namespace modelinspector {

class ViewItem
{
public:
    enum Type {
        Blockpic,
        BlockpicView1,
        BlockpicView2,
        BlockpicView3,
        Statistic,
        Unknown
    };

public:
    explicit ViewItem(const QString &name, int page, ViewItem *parent = nullptr);
    ~ViewItem();

    void append(ViewItem *child);

    ViewItem *child(int row);

    int childCount() const;

    int columnCount() const {
        return 1;
    }

    QString name() const {
        return mName;
    }

    void setName(const QString &name) {
        mName = name;
    }

    int page() const {
        return mPage;
    }

    void setPage(int page) {
        mPage = page;
    }

    int row() const;

    int type() const {
        return mType;
    }

    void setType(int type) {
        mType = type;
    }

    ViewItem *parent();

private:
    QString mName;
    ViewItem *mParent;
    int mPage;
    int mType = Unknown;
    QVector<ViewItem*> mChilds;
};

class SectionTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SectionTreeModel(QObject *parent = nullptr);
    ~SectionTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void loadModelData();

private:
    ViewItem *mRoot;
};

}
}
}

#endif // SECTIONTREEMODEL_H
