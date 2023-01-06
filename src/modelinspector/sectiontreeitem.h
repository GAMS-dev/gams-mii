#ifndef SECTIONTREEITEM_H
#define SECTIONTREEITEM_H

#include <QString>
#include <QVector>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class SectionTreeItem
{
public:
    explicit SectionTreeItem(const QString &name,
                             SectionTreeItem *parent = nullptr);

    explicit SectionTreeItem(const QString &name, int page,
                             SectionTreeItem *parent = nullptr);
    ~SectionTreeItem();

    void append(SectionTreeItem *child);

    void remove(int index, int count);

    SectionTreeItem *child(int row);

    int childCount() const;

    int columnCount() const;

    QString name() const;

    void setName(const QString &name);

    int page() const;

    void setPage(int page);

    int row() const;

    ViewDataType type() const;

    void setType(ViewDataType type);

    void setType(const QString &text);

    SectionTreeItem *parent();

private:
    QString mName;
    SectionTreeItem *mParent;
    int mPage = -1;
    ViewDataType mType = ViewDataType::Unknown;
    QVector<SectionTreeItem*> mChilds;
};

}
}
}

#endif // SECTIONTREEITEM_H
