#ifndef SEARCHRESULTMODEL_H
#define SEARCHRESULTMODEL_H

#include "searchresult.h"

#include <QAbstractTableModel>

class QStandardItem;

namespace gams {
namespace studio {
namespace modelinspector {

class SearchResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SearchResultModel(QObject *parent = nullptr);
    ~SearchResultModel();

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    void updateData(const QList<SearchResult> &data);

    SearchResult entry(int index);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    const QStringList mHeaderData { "Index", "Orientation", "Text"};
    QList<SearchResult> mData;
};

}
}
}

#endif // SEARCHRESULTMODEL_H
