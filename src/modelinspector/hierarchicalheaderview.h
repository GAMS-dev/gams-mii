#ifndef HIERARCHICALHEADERVIEW_H
#define HIERARCHICALHEADERVIEW_H

#include <QHeaderView>

#include "common.h"
#include "aggregation.h"

class QMenu;

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;
class LabelFilterWidget;

class HierarchicalHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    HierarchicalHeaderView(Qt::Orientation orientation,
                           QSharedPointer<AbstractModelInstance> modelInstance,
                           QWidget *parent = nullptr);
    ~HierarchicalHeaderView();

    QSharedPointer<AbstractModelInstance> modelInstance() const;

    void setIdentifierState(const IdentifierState &state);

    void setAppliedAggregation(const Aggregation &appliedAggregation);

    void setModel(QAbstractItemModel *model) override;

    ViewDataType viewType() const;
    void setViewType(ViewDataType viewType);

public slots:
    void customMenuRequested(const QPoint &position);
    void resetSymbolLabelFilters();

signals:
    void filterChanged(const gams::studio::modelinspector::IdentifierState&, Qt::Orientation);

private slots:
    void on_filterChanged(const gams::studio::modelinspector::IdentifierState& state,
                          Qt::Orientation orientation);

protected:
    void paintSection(QPainter *painter, const QRect &rect,
                      int logicalIndex) const override;

    QSize sectionSizeFromContents(int logicalIndex) const override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    QStyleOptionHeader styleOptionForCell(int logicalIndex) const;

private:
    class HierarchicalHeaderView_private;
    HierarchicalHeaderView_private *mPrivate;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    ViewDataType mViewType = ViewDataType::Unknown;

    QMenu *mFilterMenu;
    LabelFilterWidget *mFilterWidget;
};

}
}
}

#endif // HIERARCHICALHEADERVIEW_H
