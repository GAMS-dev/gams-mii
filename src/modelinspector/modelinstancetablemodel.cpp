#include "modelinstancetablemodel.h"
#include "modelinstance.h"
#include "hierarchicalheaderview.h"

#include <functional>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

ModelInstanceTableModel::ModelInstanceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

ModelInstanceTableModel::~ModelInstanceTableModel()
{
}

void ModelInstanceTableModel::setModelInstance(const QSharedPointer<ModelInstance> &modelInstance)
{
    beginResetModel();
    mHorizontalHeaderModel.clear();
    mVerticalHeaderModel.clear();
    mModelInstance = modelInstance;
    mModelInstance->horizontalHeaderData(mHorizontalHeaderModel);
    mModelInstance->verticalHeaderData(mVerticalHeaderModel);
    endResetModel();
}

QVariant ModelInstanceTableModel::data(const QModelIndex &index, int role) const
{
    if (role == HierarchicalHeaderView::HorizontalHeaderDataRole) {
        QVariant headerModel;
        headerModel.setValue((QObject*)&mHorizontalHeaderModel);
        return headerModel;
    }

    if (role == HierarchicalHeaderView::VerticalHeaderDataRole) {
        QVariant variant;
        variant.setValue((QObject*)&mVerticalHeaderModel);
        return variant;
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }

    if (index.isValid() && role == Qt::DisplayRole) {
        return mModelInstance->data(index.row(), index.column());
    }

    return QVariant();
}

Qt::ItemFlags ModelInstanceTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex ModelInstanceTableModel::index(int row, int column,
                                           const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

QModelIndex ModelInstanceTableModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ModelInstanceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount();
}

int ModelInstanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount();
}

QList<SearchResult> ModelInstanceTableModel::searchHeaders(const QString &term,
                                                           bool isRegEx)
{
    QList<SearchResult> result;
    find(term, isRegEx, Qt::Horizontal, mHorizontalHeaderModel, result);
    find(term, isRegEx, Qt::Vertical, mVerticalHeaderModel, result);
    return result;
}

void ModelInstanceTableModel::find(const QString &term,
                                   bool isRegEx,
                                   Qt::Orientation orientation,
                                   const QStandardItemModel &headerModel,
                                   QList<SearchResult> &result)
{
    std::function<bool(const QString&)> compare;
    if (isRegEx) {
        QRegExp regex(term);
        compare = [regex](const QString &text) {
            return regex.exactMatch(text);
        };
    } else {
        compare = [&term](const QString &text) {
            return text.contains(term, Qt::CaseInsensitive);
        };
    }

    QList<QStandardItem*> items;
    for (int c=0; c<headerModel.columnCount(); ++c) {
        items.append(headerModel.item(0, c));
    }
    while (!items.isEmpty()) {
        QList<QStandardItem*> newItems;
        while (!items.isEmpty()) {
            auto item = items.takeFirst();
            if (compare(item->text()) && item->checkState() != Qt::Unchecked) {
                if (orientation == Qt::Horizontal) {
                    auto sr = SearchResult { mModelInstance->horizontalIndex(item),
                                             orientation,
                                             item };
                    result.append(sr);
                } else {
                    auto sr = SearchResult { mModelInstance->verticalIndex(item),
                                             orientation,
                                             item };
                    result.append(sr);
                }
            }
            for (int c=0; c<item->columnCount(); ++c) {
                auto child = item->child(0, c);
                if (child)
                    newItems.append(child);
            }
        }
        items.append(newItems);
    }
}

}
}
}
