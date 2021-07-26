#ifndef MITABLEVIEW_H
#define MITABLEVIEW_H

#include <QTableView>

namespace gams {
namespace studio{
namespace modelinspector {

class MITableView : public QTableView
{// TODO keep?
    Q_OBJECT

public:
    MITableView(QWidget *parent = nullptr);

};

}
}
}

#endif // MITABLEVIEW_H
