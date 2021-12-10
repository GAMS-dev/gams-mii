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
                             int index = -1,
                             FilterTreeItem *parent = nullptr);

    ~FilterTreeItem();

    void append(FilterTreeItem *child);

    FilterTreeItem* child(int index);

    QList<FilterTreeItem*> childs() const;

    bool hasChildren() const;

    int columnCount() const;

    int rowCount() const;

    int row() const;

    FilterTreeItem* parent() const;
    void setParent(FilterTreeItem *parent);

    QString text() const;
    void setText(const QString &text);

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    Qt::CheckState checked();
    void setChecked(Qt::CheckState state);

    void setSubTreeState(Qt::CheckState checked);

    int index() const;
    void setIndex(int index);

    int symbolIndex() const;
    void setSymbolIndex(int index);

    static const QString EquationText;
    static const QString VariableText;

private:
    FilterTreeItem *mParent = nullptr;

    QList<FilterTreeItem*> mChilds;

    QString mText;

    Qt::CheckState mChecked;

    ///
    /// \brief mIndex Logical, section or symbol index.
    ///
    int mIndex;

    int mSymbolIndex;

    bool mCheckable = true;
    bool mEnabled = true;
};

}
}
}

#endif // FILTERTREEITEM_H
