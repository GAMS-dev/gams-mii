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

    SectionTreeItem *child(int row);

    int childCount() const;

    int columnCount() const {
        return 1;
    }

    QString name() const {
        return mName;
    }

    void setName(const QString &name) {
        mName = name;
    }

    int page() const {
        return mPage;
    }

    void setPage(int page) {
        mPage = page;
    }

    int row() const;

    PredefinedViewEnum type() const {
        return mType;
    }

    void setType(PredefinedViewEnum type) {
        mType = type;
    }

    void setType(const QString &text);

    SectionTreeItem *parent();

private:
    QString mName;
    SectionTreeItem *mParent;
    int mPage = -1;
    PredefinedViewEnum mType = PredefinedViewEnum::Unknown;
    QVector<SectionTreeItem*> mChilds;
};

}
}
}

#endif // SECTIONTREEITEM_H
