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
    enum HeaderDataModelRoles
    {
        HorizontalHeaderDataRole = Qt::UserRole,
        VerticalHeaderDataRole = Qt::UserRole+1
    };

    HierarchicalHeaderView(Qt::Orientation orientation,
                           QWidget *parent = nullptr);
    ~HierarchicalHeaderView();

    void setModel(QAbstractItemModel *model) override;
    void setModelInstance(QSharedPointer<ModelInstance> modelInstance);

public slots:
    void customMenuRequested(QPoint position);

signals:
    void filterChanged(FilterTreeItem *item, Qt::Orientation orientation);

protected:
    void paintSection(QPainter *painter, const QRect &rect,
                      int logicalIndex) const override;
    QSize sectionSizeFromContents(int logicalIndex) const override;

    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void on_sectionResized(int logicalIndex, int oldSize, int newSize);

private:
    QStyleOptionHeader styleOptionForCell(int logicalIndex) const;

    bool isEquation(const QString &name) const;
    bool isVariable(const QString &name) const;

    FilterTreeItem* buildFilterItems(QStandardItem *item);
    void appendChildren(FilterTreeItem *parent, QStandardItem *item);

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
