#include "search.h"
#include "common.h"
#include "modelinstance.h"

#include <QRegExp>
#include <QAbstractItemModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

Search::Search(QSharedPointer<AbstractModelInstance> modelInstance,
               QAbstractItemModel *dataModel,
               const Aggregation &appliedAggregation,
               const QString &term, bool isRegEx)
    : Search(modelInstance, dataModel, term, isRegEx)
{
    mAppliedAggregation = appliedAggregation;
}

Search::Search(QSharedPointer<AbstractModelInstance> modelInstance,
               QAbstractItemModel *dataModel,
               const QString &term, bool isRegEx)
    : mModelInstance(modelInstance)
    , mDataModel(dataModel)
{
    if (isRegEx) {
        QRegExp regex(term);
        compare = [regex](const QString &text) {
            return regex.exactMatch(text);
        };
    } else {
        compare = [&term](const QString &text) {
            return text.contains(term, Qt::CaseInsensitive);
        };
    }
}

void Search::run(QList<SearchResult> &result)
{
    if (!mModelInstance || !mDataModel) return;
    if (mAppliedAggregation.isActive() && mAppliedAggregation.type() == Aggregation::MinMax) {
        searchMinMax(Qt::Horizontal, result);
        searchMinMax(Qt::Vertical, result);
    } else if (mAppliedAggregation.isActive()) {
        searchAggregated(Qt::Horizontal, result);
        searchAggregated(Qt::Vertical, result);
    } else {
        search(Qt::Horizontal, result);
        search(Qt::Vertical, result);
    }
}

void Search::search(Qt::Orientation orientation, QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        if (!ok) continue;
        searchHeader(section, realSection, orientation, result);
    }
}

void Search::searchHeader(int logicalIndex, int sectionIndex,
                          Qt::Orientation orientation, QList<SearchResult> &result)
{
    if (sectionIndex < PredefinedHeaderLength && compare(PredefinedHeader.at(sectionIndex))) {
        result.append(SearchResult{logicalIndex, orientation});
        return;
    }

    auto sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                             : mModelInstance->equation(sectionIndex);
    if (compare(sym.name())) {
        result.append(SearchResult{logicalIndex, orientation});
    } else {
        auto labels = sym.sectionLabels()[sectionIndex];
        Q_FOREACH(const auto label, labels) {
            if (compare(label)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

void Search::searchMinMax(Qt::Orientation orientation, QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        if (!ok) continue;
        searchMinMaxHeader(section, realSection, orientation, result);
    }
}

void Search::searchMinMaxHeader(int logicalIndex,
                                    int sectionIndex,
                                    Qt::Orientation orientation,
                                    QList<SearchResult> &result)
{
    Symbol sym;
    if (mAppliedAggregation.indexToSymbol(orientation).contains(sectionIndex)) {
        sym = mAppliedAggregation.indexToSymbol(orientation).value(sectionIndex);
    } else {
        sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                            : mModelInstance->equation(sectionIndex);
    }
    if (orientation == Qt::Horizontal && compare(sym.name())) {
        result.append(SearchResult{logicalIndex, orientation});
    } else if (orientation == Qt::Vertical) {
        int index = sectionIndex;
        if (!mAppliedAggregation.aggregationSymbols(Qt::Vertical).contains(index)) {
            --index;
        }
        auto item = mAppliedAggregation.aggregationSymbols(Qt::Vertical).value(index);
        Q_FOREACH(const auto label, item.labels()[sectionIndex]) {
            if (compare(item.text()) || compare(label)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

void Search::searchAggregated(Qt::Orientation orientation, QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        if (!ok) continue;
        searchAggregatedHeader(section, realSection, orientation, result);
    }
}

void Search::searchAggregatedHeader(int logicalIndex, int sectionIndex,
                                    Qt::Orientation orientation, QList<SearchResult> &result)
{
    if (sectionIndex < PredefinedHeaderLength && compare(PredefinedHeader.at(sectionIndex))) {
        result.append(SearchResult{logicalIndex, orientation});
        return;
    }

    Symbol sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                               : mModelInstance->equation(sectionIndex);
    if (!mAppliedAggregation.indexToSymbol(orientation).contains(sectionIndex) &&
            !mAppliedAggregation.indexToSymbol(orientation).contains(sym.firstSection())) {
        searchHeader(logicalIndex, sectionIndex, orientation, result);
        return;
    }
    auto item = mAppliedAggregation.aggregationSymbols(orientation).value(sym.firstSection());
    Q_FOREACH(const auto label, item.labels()[sectionIndex]) {
        if (compare(item.text()) || compare(label)) {
            result.append(SearchResult{logicalIndex, orientation});
            break;
        }
    }
}

}
}
}
