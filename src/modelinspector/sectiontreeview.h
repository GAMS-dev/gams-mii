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
