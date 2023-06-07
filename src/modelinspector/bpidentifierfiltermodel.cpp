#include "bpidentifierfiltermodel.h"
#include "abstractmodelinstance.h"

namespace gams {
namespace studio{
namespace modelinspector {

BPIdentifierFilterModel::BPIdentifierFilterModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                                 QObject *parent)
    : QSortFilterProxyModel(parent)
    , mModelInstance(modelInstance)
{

}

void BPIdentifierFilterModel::setIdentifierFilter(const IdentifierFilter &filter)
{
    mIdentifierFilter = filter;
    invalidateFilter();
}

bool BPIdentifierFilterModel::filterAcceptsColumn(int sourceColumn,
                                                  const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceColumn, Qt::Horizontal,
                                                  Mi::IndexDataRole).toInt(&ok);
    if (!ok || sectionIndex < 0)
        return true;
    for (auto iter=mIdentifierFilter[Qt::Horizontal].constBegin();
         iter!=mIdentifierFilter[Qt::Horizontal].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto variable = mModelInstance->variable(sectionIndex);
        if (iter->Text == variable->name()) {
            return false;
        }
    }
    return true;
}

bool BPIdentifierFilterModel::filterAcceptsRow(int sourceRow,
                                               const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    bool ok;
    auto sectionIndex = sourceModel()->headerData(sourceRow, Qt::Vertical,
                                                  Mi::IndexDataRole).toInt(&ok);
    if (!ok || sectionIndex < 0)
        return true;
    for (auto iter=mIdentifierFilter[Qt::Vertical].constBegin();
         iter!=mIdentifierFilter[Qt::Vertical].constEnd(); ++iter) {
        if (iter->Checked == Qt::Checked) {
            continue;
        }
        auto equation = mModelInstance->equation(sectionIndex);
        if (iter->Text == equation->name()) {
            return false;
        }
    }
    return true;
}

}
}
}
