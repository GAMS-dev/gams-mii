#ifndef SEARCHRESULTVIEW_H
#define SEARCHRESULTVIEW_H

#include "abstracttableview.h"

namespace gams {
namespace studio{
namespace modelinspector {

class SearchResultView : public AbstractTableView
{
    Q_OBJECT

public:
    SearchResultView(QWidget *parent = nullptr);
};

}
}
}

#endif // SEARCHRESULTVIEW_H
