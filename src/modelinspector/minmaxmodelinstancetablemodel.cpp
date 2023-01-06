#include "minmaxmodelinstancetablemodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

MinMaxIdentifierFilterModel::MinMaxIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                         QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void MinMaxIdentifierFilterModel::setIdentifierFilter(const IdentifierFilter &filter,
                                                      const Aggregation &appliedAggregation)
{
    mIdentifierFilter = filter;
    mAppliedAggregation = appliedAggregation;
    invalidateFilter();
}

bool MinMaxIdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
                                                      const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal).toInt(&ok);
    if (!ok) return true;

    for (auto iter=mIdentifierFilter[Qt::Horizontal].constBegin(); iter!=mIdentifierFilter[Qt::Horizontal].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        if (iter->Text == sym->name() && sectionIndex >= sym->firstSection() &&
                sectionIndex <= sym->lastSection()) {
            return false;
        }
    }
    return true;
}

bool MinMaxIdentifierFilterModel::filterAcceptsRow(int sourceRow,
                                                   const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical).toInt(&ok);
    if (!ok) return true;

    for (auto iter=mIdentifierFilter[Qt::Vertical].constBegin(); iter!=mIdentifierFilter[Qt::Vertical].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        Symbol *sym;
        if (mAppliedAggregation.indexToEquation().contains(sectionIndex))
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex);
        else
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex-1);
        if (iter->Text == sym->name()) {
            return false;
        }
    }
    return true;
}

MinMaxModelInstanceTableModel::MinMaxModelInstanceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

MinMaxModelInstanceTableModel::~MinMaxModelInstanceTableModel()
{

}

void MinMaxModelInstanceTableModel::setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance)
{
    beginResetModel();
    mModelInstance = modelInstance;
    endResetModel();
}

QVariant MinMaxModelInstanceTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    if (role == Qt::DisplayRole && index.isValid()) {
        return mModelInstance->data(index.row(), index.column(), mView);
    }
    return QVariant();
}

Qt::ItemFlags MinMaxModelInstanceTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractTableModel::flags(index) |
                Qt::ItemIsEnabled |
                Qt::ItemIsSelectable;
    return Qt::NoItemFlags;
}

QVariant MinMaxModelInstanceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        auto realIndex = mModelInstance->headerData(section, orientation, mView);
        return realIndex < 0 ? QVariant() : realIndex;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex MinMaxModelInstanceTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
        return createIndex(row, column);
    return QModelIndex();
}

int MinMaxModelInstanceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->rowCount(ViewDataType::MinMax);
}

int MinMaxModelInstanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(ViewDataType::MinMax);
}

int MinMaxModelInstanceTableModel::view() const
{
    return mView;
}

void MinMaxModelInstanceTableModel::setView(int view)
{
    mView = view;
}

}
}
}
