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
    explicit FilterTreeItem(const QString &text = QString(),
                            Qt::CheckState checkState = Qt::Checked,
                            FilterTreeItem *parent = nullptr);

    ~FilterTreeItem();

    FilterTreeItem* findChild(const QString &text);

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

    ///
    /// \brief Set sub tree state, i.e. set the state for all child items.
    /// \param Check state for childs items.
    ///
    void setSubTreeState(Qt::CheckState checked);

    int sectionIndex() const;
    void setSectionIndex(int index);

    int symbolIndex() const;
    void setSymbolIndex(int index);

    bool isVisible() const;
    void setVisible(bool visible);

    static const QString EquationText;
    static const QString VariableText;

private:
    FilterTreeItem *mParent = nullptr;

    QList<FilterTreeItem*> mChilds;

    QString mText;

    Qt::CheckState mChecked;

    int mSectionIndex = -1;

    int mSymbolIndex = -1;

    bool mCheckable = true;
    bool mEnabled = true;
    bool mVisible = true;
};

}
}
}

#endif // FILTERTREEITEM_H
