/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SECTIONTREEITEM_H
#define SECTIONTREEITEM_H

#include <QString>
#include <QVector>

#include "common.h"

namespace gams {
namespace studio {
namespace mii {

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
