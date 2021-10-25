#include "searchresultmodel.h"

namespace gams {
namespace studio {
namespace modelinspector {

SearchResultModel::SearchResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

SearchResultModel::~SearchResultModel()
{

}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    if (index.row() >= mData.size())
        return QVariant();

    if (index.column() == 0) {
        return mData[index.row()].Index;
    }
    if (index.column() == 1) {
        return mData[index.row()].Orientation == Qt::Horizontal? "Horizontal" : "Vertical";
    }
    return QVariant();
}

void SearchResultModel::updateData(const QList<SearchResult> &data)
{
    beginResetModel();
    mData = data;
    endResetModel();
}

SearchResult SearchResultModel::entry(int index)
{
    if (index >= mData.size())
        return SearchResult();
    return mData.at(index);
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return mHeaderData.at(section);
    }
    return QVariant();
}

int SearchResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mHeaderData.size();
}

int SearchResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.size();
}

}
}
}
