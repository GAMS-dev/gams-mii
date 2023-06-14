#ifndef SEARCH_H
#define SEARCH_H

#include <functional>
#include <QSharedPointer>
#include <QString>

class QAbstractItemModel;

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;
class AbstractViewConfiguration;
struct SearchResult;

class Search
{
public:
    Search(QSharedPointer<AbstractViewConfiguration> viewConfig,
           QAbstractItemModel *dataModel,
           const QString &term,
           bool isRegEx);

    void run();

private:
    void searchStaticHeader(Qt::Orientation orientation);
    void searchHeaderHierarchy(Qt::Orientation orientation);
    void searchHeaderHierarchy(int logicalIndex, int sectionIndex,
                               Qt::Orientation orientation);

private:
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
    QAbstractItemModel *mDataModel;
    std::function<bool(const QString&)> compare;
};

}
}
}

#endif // SEARCH_H
