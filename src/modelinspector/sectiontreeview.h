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
#ifndef SECTIONTREEVIEW_H
#define SECTIONTREEVIEW_H

#include "common.h"

#include <QTreeView>

class QAction;
class QMenu;

namespace gams {
namespace studio {
namespace modelinspector {

class SectionTreeView : public QTreeView
{
    Q_OBJECT

public:
    SectionTreeView(QWidget *parent = nullptr);

    ViewActionStates viewActionStates() const;

signals:
    void currentItemChanged(int);

    void saveViewTriggered();
    void removeViewTriggered();

public slots:
    void showCustomContextMenu(const QPoint &pos);

protected:
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous) override;

private:
    ViewActionStates viewActionStates(const QModelIndex &index) const;

private:
    QMenu* mMenu;
    QAction* mSaveViewAction;
    QAction* mRemoveViewAction;
};

}
}
}

#endif // SECTIONTREEVIEW_H
