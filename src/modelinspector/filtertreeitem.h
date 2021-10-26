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
                            bool checkable,
                            FilterTreeItem *parent = nullptr);

    explicit FilterTreeItem(const QString &text = "",
                             Qt::CheckState checkState = Qt::Checked,
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

    static const QString EquationText;
    static const QString VariableText;

private:
    FilterTreeItem *mParent = nullptr;

    QList<FilterTreeItem*> mChilds;

    QString mText;

    Qt::CheckState mChecked;

    int mLogicalIndex;

    bool mCheckable = true;
};

}
}
}

#endif // FILTERTREEITEM_H
