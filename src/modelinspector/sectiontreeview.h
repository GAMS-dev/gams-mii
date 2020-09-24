#ifndef SECTIONTREEVIEW_H
#define SECTIONTREEVIEW_H

#include <QTreeView>

namespace gams {
namespace studio {
namespace modelinspector {

class SectionTreeView : public QTreeView
{
    Q_OBJECT

public:
    SectionTreeView(QWidget *parent = nullptr);

signals:
    void currentItemChanged(int);

protected:
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous) override;
};

}
}
}

#endif // SECTIONTREEVIEW_H
