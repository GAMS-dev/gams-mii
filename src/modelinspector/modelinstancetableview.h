#ifndef MODELINSTANCETABLEVIEW_H
#define MODELINSTANCETABLEVIEW_H

#include <QTableView>

namespace gams {
namespace studio{
namespace modelinspector {

class ModelInstanceTableView : public QTableView
{
    Q_OBJECT

public:
    ModelInstanceTableView(QWidget *parent = nullptr);
};

}
}
}

#endif // MODELINSTANCETABLEVIEW_H
