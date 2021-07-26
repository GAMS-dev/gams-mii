#ifndef FILTERTREEITEM_H
#define FILTERTREEITEM_H

#include <QString>
#include <QList>

namespace gams {
namespace studio {
namespace modelinspector {

class FilterTreeItem
{
public:
    explicit FilterTreeItem(const QString &text,
                             Qt::CheckState checkState,
                             int logicalIndex = -1,
                             FilterTreeItem *parent = nullptr);
    ~FilterTreeItem();

    void append(FilterTreeItem *child);

    FilterTreeItem* child(int row);

    QList<FilterTreeItem*> childs() const;

    int columnCount() const;

    int rowCount() const;

    int row() const;

    FilterTreeItem* parent();

    QString text() const;

    bool isCheckable() const;

    void setCheckable(bool checkable);

    Qt::CheckState checked();

    void setChecked(Qt::CheckState state);

    void setSubTreeState(Qt::CheckState checked);

    int logicalIndex() const;

    void setLogicalIndex(int index);

    static const QString HeaderText;

private:
    FilterTreeItem *mParent = nullptr;

    QList<FilterTreeItem*> mChilds;

    QString mText;

    Qt::CheckState mChecked = Qt::Checked;

    int mLogicalIndex;

    bool mCheckable = true;
};

}
}
}

#endif // FILTERTREEITEM_H
