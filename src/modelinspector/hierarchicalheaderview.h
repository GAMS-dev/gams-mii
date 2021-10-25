#ifndef HIERARCHICALHEADERVIEW_H
#define HIERARCHICALHEADERVIEW_H

#include <QHeaderView>

class QMenu;
class QStandardItem;

namespace gams {
namespace studio{
namespace modelinspector {

class FilterTreeItem;
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

    void setModel(QAbstractItemModel *model) override;

public slots:
    void customMenuRequested(QPoint position);

signals:
    void filterChanged(FilterTreeItem *item, Qt::Orientation orientation);

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
