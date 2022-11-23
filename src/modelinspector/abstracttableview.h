#ifndef ABSTRACTTABLEVIEW_H
#define ABSTRACTTABLEVIEW_H

#include <QTableView>

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractTableView : public QTableView
{
    Q_OBJECT

public:
    AbstractTableView(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    void resetZoom();

private:
    void zoom(int range);

private:
    QFont mBaseFont;
};

}
}
}

#endif // ABSTRACTTABLEVIEW_H
