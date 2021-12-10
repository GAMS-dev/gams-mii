#ifndef HIERARCHICALHEADERVIEW_H
#define HIERARCHICALHEADERVIEW_H

#include <QHeaderView>

#include "common.h"
#include "aggregation.h"

class QMenu;

namespace gams {
namespace studio{
namespace modelinspector {

class ModelInstance;
class LabelFilterWidget;

class HierarchicalHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    HierarchicalHeaderView(Qt::Orientation orientation,
                           QSharedPointer<ModelInstance> modelInstance,
                           QWidget *parent = nullptr);
    ~HierarchicalHeaderView();

    QSharedPointer<ModelInstance> modelInstance() const;

    void setAppliedAggregation(const AggregationSymbols &appliedAggregation);

    void setModel(QAbstractItemModel *model) override;

public slots:
    void customMenuRequested(QPoint position);
    void resetSymbolLabelFilters();

signals:
    void filterChanged(const IdentifierState&, Qt::Orientation);

private slots:
    void on_filterChanged(const IdentifierState& state,
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
    QSharedPointer<ModelInstance> mModelInstance;

    QMenu *mFilterMenu;
    LabelFilterWidget *mFilterWidget;
};

}
}
}

#endif // HIERARCHICALHEADERVIEW_H
