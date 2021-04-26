#ifndef HIERARCHICALHEADERVIEW_H
#define HIERARCHICALHEADERVIEW_H

#include <QHeaderView>

namespace gams {
namespace studio{
namespace modelinspector {

class HierarchicalHeaderView : public QHeaderView
{
public:
    enum HeaderDataModelRoles
    {
        HorizontalHeaderDataRole = Qt::UserRole,
        VerticalHeaderDataRole = Qt::UserRole+1
    };

    HierarchicalHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~HierarchicalHeaderView();

    void setModel(QAbstractItemModel *model) override;

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    QSize sectionSizeFromContents(int logicalIndex) const override;

private slots:
    void on_sectionResized(int logicalIndex, int oldSize, int newSize);

private:
    QStyleOptionHeader styleOptionForCell(int logicalIndex) const;

private:
    class HierarchicalHeaderView_private;
    HierarchicalHeaderView_private *mPrivate;
};

}
}
}

#endif // HIERARCHICALHEADERVIEW_H
