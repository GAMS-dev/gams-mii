#include "modelinspector.h"
#include "ui_modelinspector.h"

namespace gams {
namespace studio{
namespace modelinspector {

ModelInspector::ModelInspector(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ModelInspector)
{
    ui->setupUi(this);
}

}
}
}
