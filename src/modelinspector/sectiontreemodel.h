#ifndef SECTIONTREEMODEL_H
#define SECTIONTREEMODEL_H

#include <QAbstractItemModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class SectionTreeItem;

class SectionTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SectionTreeModel(QObject *parent = nullptr);
    virtual ~SectionTreeModel();

    void appendCustomView(const QString &text, PredefinedViewEnum type, int page);
    void removeCustomView();

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

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

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void loadModelData();

private:
    SectionTreeItem *mRoot;
    SectionTreeItem *mCustomRoot;
};

}
}
}

#endif // SECTIONTREEMODEL_H
