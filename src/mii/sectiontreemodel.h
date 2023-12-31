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
#ifndef SECTIONTREEMODEL_H
#define SECTIONTREEMODEL_H

#include <QAbstractItemModel>

#include "common.h"

class QStackedWidget;

namespace gams {
namespace studio {
namespace mii {

class AbstractViewFrame;
class SectionTreeItem;

class SectionTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum ItemRole {
        ItemDataTypeRole = Qt::UserRole
    };

    explicit SectionTreeModel(QObject *parent = nullptr);

    virtual ~SectionTreeModel();

    void appendCustomView(const QString &text, ViewHelper::ViewDataType type, AbstractViewFrame* widget);

    SectionTreeItem* data() const;

    SectionTreeItem* customData() const;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QList<AbstractViewFrame*> removeItem(SectionTreeItem* item);

    QList<AbstractViewFrame*> removeCustomRows();

    void loadModelData(QStackedWidget* stackedWidget);

private:
    SectionTreeItem *mRoot;
    SectionTreeItem *mCustomRoot;
    SectionTreeItem *mCustomBlockpic = nullptr;
    SectionTreeItem *mCustomPostopt = nullptr;
    SectionTreeItem *mCustomSymbolView = nullptr;
};

}
}
}

#endif // SECTIONTREEMODEL_H
