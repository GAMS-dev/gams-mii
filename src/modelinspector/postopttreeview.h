#ifndef POSTOPTTREEVIEW_H
#define POSTOPTTREEVIEW_H

#include <QTreeView>

namespace gams {
namespace studio{
namespace modelinspector {

class PostoptTreeView : public QTreeView
{
public:
    PostoptTreeView(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    void resetZoom();

private:
    void zoom(int range);

    void resizeColumns();

private:
    QFont mBaseFont;
};

}
}
}

#endif // POSTOPTTREEVIEW_H
