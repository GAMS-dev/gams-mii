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
    Search(QSharedPointer<AbstractModelInstance> modelInstance,
           QSharedPointer<AbstractViewConfiguration> viewConfig,
           QAbstractItemModel *dataModel,
           const QString &term,
           bool isRegEx);

    void run(QList<SearchResult> &result);

private:
    void searchPlainHeader(Qt::Orientation orientation, QList<SearchResult> &result);

    void searchHeaderHierarchy(Qt::Orientation orientation, QList<SearchResult> &result);
    void searchHeaderHierarchy(int logicalIndex, int sectionIndex,
                               Qt::Orientation orientation, QList<SearchResult> &result);
    void searchFixedHeaderHierarchy(int logicalIndex, int sectionIndex,
                                    Qt::Orientation orientation, QList<SearchResult> &result);

private:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
    QAbstractItemModel *mDataModel;
    std::function<bool(const QString&)> compare;
};

}
}
}

#endif // SEARCH_H
