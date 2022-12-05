#ifndef SEARCH_H
#define SEARCH_H

#include "aggregation.h"

#include <functional>
#include <QSharedPointer>
#include <QString>

class QAbstractItemModel;

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;
struct SearchResult;

class Search
{
public:
    Search(QSharedPointer<AbstractModelInstance> modelInstance,
           QAbstractItemModel *dataModel,
           const Aggregation &appliedAggregation,
           const QString &term, bool isRegEx);

    Search(QSharedPointer<AbstractModelInstance> modelInstance,
           QAbstractItemModel *dataModel,
           const QString &term, bool isRegEx);

    void run(QList<SearchResult> &result);

private:
    void search(Qt::Orientation orientation, QList<SearchResult> &result);
    void searchHeader(int logicalIndex, int sectionIndex,
                      Qt::Orientation orientation, QList<SearchResult> &result);

    void searchMinMax(Qt::Orientation orientation, QList<SearchResult> &result);
    void searchMinMaxHeader(int logicalIndex, int sectionIndex,
                                Qt::Orientation orientation, QList<SearchResult> &result);

    void searchAggregated(Qt::Orientation orientation, QList<SearchResult> &result);
    void searchAggregatedHeader(int logicalIndex, int sectionIndex,
                                Qt::Orientation orientation, QList<SearchResult> &result);

private:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QAbstractItemModel *mDataModel;
    Aggregation mAppliedAggregation;
    std::function<bool(const QString&)> compare;
};

}
}
}

#endif // SEARCH_H
