#ifndef MODELINSPECTOR_H
#define MODELINSPECTOR_H

#include <QWidget>

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class ModelInspector;
}

class ModelInspector : public QWidget
{
    Q_OBJECT

public:
    explicit ModelInspector(QWidget *parent = nullptr);

private:
    Ui::ModelInspector* ui;
};

}
}
}

#endif // MODELINSPECTOR_H
