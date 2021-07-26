#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QStandardItem>

namespace gams {
namespace studio {
namespace modelinspector {

struct SearchResult { // TODO keep or replace with FilterTreeItem?
    int Index = -1;
    Qt::Orientation Orientation;
    QStandardItem* ModelItem = nullptr;

    bool isValid() const {
        return Index >= 0 || ModelItem;
    }
};

}
}
}

#endif // SEARCHRESULT_H
