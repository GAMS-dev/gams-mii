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

IdentifierFilter& MinMaxIdentifierFilterModel::identifierFilter()
{
    return mIdentifierFilter;
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

    Q_FOREACH(const auto& item, mIdentifierFilter[Qt::Horizontal]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sectionIndex < constant->PredefinedHeaderLength &&
                item.SymbolIndex == sectionIndex) {
            return false;
        }
        auto sym = mModelInstance->variable(sectionIndex);
        if (item.Text == sym.name() && sectionIndex >= sym.firstSection() &&
                sectionIndex <= sym.lastSection()) {
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

    Q_FOREACH(const auto& item, mIdentifierFilter[Qt::Vertical]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sectionIndex < constant->PredefinedHeaderLength &&
                item.SymbolIndex == sectionIndex) {
            return false;
        }
        Symbol sym;
        if (mAppliedAggregation.indexToEquation().contains(sectionIndex))
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex);
        else
            sym = mAppliedAggregation.indexToEquation().value(sectionIndex-1);
        if (item.Text == sym.name()) {
            return false;
        }
    }
    return true;
}

MinMaxModelInstanceTableModel::MinMaxModelInstanceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , mViewType(PredefinedViewEnum::MinMax)
{

}

MinMaxModelInstanceTableModel::~MinMaxModelInstanceTableModel()
{

}

const Aggregation& MinMaxModelInstanceTableModel::aggregation() const
{
    return mAggregation;
}

const Aggregation& MinMaxModelInstanceTableModel::appliedAggregation() const
{
    return mAppliedAggregation;
}

void MinMaxModelInstanceTableModel::setAggregation(const Aggregation &aggregation,
                                                   const Aggregation &appliedAggregation)
{
    beginResetModel();
    mAggregation = aggregation;
    mAppliedAggregation = appliedAggregation;
    mModelInstance->aggregate(mAppliedAggregation);
    endResetModel();
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
    if (role == Qt::DisplayRole && mAppliedAggregation.isActive()) {
        if (!index.isValid())
            return QVariant();
        QVariant value = mModelInstance->data(index.row(), index.column(), mView);
        return value == 0.0 ? QVariant() : value;
    }
    if (index.isValid() && role == Qt::DisplayRole) {
        auto value = mModelInstance->data(index.row(), index.column(), mView);
        return value == 0.0 ? QVariant() : value;
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
    if (role == Qt::DisplayRole && mAppliedAggregation.isActive()) {
        auto realIndex = mModelInstance->headerData(section, orientation, mView);
        return realIndex < 0 ? QVariant() : realIndex;
    }
    if (role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
    return section;
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
    return mModelInstance->rowCount(PredefinedViewEnum::MinMax);
}

int MinMaxModelInstanceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelInstance->columnCount(PredefinedViewEnum::MinMax);
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
