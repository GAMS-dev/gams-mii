#ifndef MODELINSTANCETABLEVIEW_H
#define MODELINSTANCETABLEVIEW_H

#include "abstracttableview.h"

namespace gams {
namespace studio{
namespace modelinspector {

class ModelInstanceTableView : public AbstractTableView
{
    Q_OBJECT

public:
    ModelInstanceTableView(QWidget *parent = nullptr);
};

}
}
}

#endif // MODELINSTANCETABLEVIEW_H
