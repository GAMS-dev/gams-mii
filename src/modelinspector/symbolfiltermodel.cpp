#include "symbolfiltermodel.h"
#include "modelinstance.h"

namespace gams {
namespace studio {
namespace modelinspector {

SymbolFilterModel::SymbolFilterModel(QSharedPointer<ModelInstance> modelInstance,
                                     QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

SymbolFilterMap& SymbolFilterModel::symbolFilter()
{
    return mSymbolFilter;
}

void SymbolFilterModel::setSymbolFilter(const SymbolFilterMap &filter)
{
    mSymbolFilter = filter;
    invalidateFilter();
}

bool SymbolFilterModel::filterAcceptsColumn(int sourceColumn,
                                            const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    Q_FOREACH(auto item, mSymbolFilter[Qt::Horizontal]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sourceColumn < PredefinedHeaderLength &&
                item.SectionIndex == sourceColumn) {
            return false;
        }
        auto sym = mModelInstance->variable(sourceColumn);
        if (item.Text == sym.Name && sourceColumn >= sym.firstSection() &&
                sourceColumn <= sym.lastSection()) {
            return false;
        }
    }
    return true;
}

bool SymbolFilterModel::filterAcceptsRow(int sourceRow,
                                         const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);
    Q_FOREACH(auto item, mSymbolFilter[Qt::Vertical]) {
        if (item.Checked == Qt::Checked) {
            continue;
        }
        if (sourceRow < PredefinedHeaderLength &&
                item.SectionIndex == sourceRow) {
            return false;
        }
        auto sym = mModelInstance->equation(sourceRow);
        if (item.Text == sym.Name && sourceRow >= sym.firstSection() &&
                sourceRow <= sym.lastSection()) {
            return false;
        }
    }
    return true;
}

}
}
}
