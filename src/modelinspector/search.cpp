#include "search.h"
#include "common.h"
#include "abstractmodelinstance.h"

#include <QAbstractItemModel>
#include <QRegularExpression>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

Search::Search(QSharedPointer<AbstractModelInstance> modelInstance,
               QAbstractItemModel *dataModel,
               const Aggregation &appliedAggregation,
               const QString &term, bool isRegEx, ViewDataType type)
    : Search(modelInstance, dataModel, term, isRegEx, type)
{
    mAppliedAggregation = appliedAggregation;
}

Search::Search(QSharedPointer<AbstractModelInstance> modelInstance,
               QAbstractItemModel *dataModel,
               const QString &term, bool isRegEx, ViewDataType type)
    : mModelInstance(modelInstance)
    , mDataModel(dataModel)
    , mViewDataType(type)
{
    if (isRegEx) {
        QRegularExpression regex(term);
        compare = [regex](const QString &text) {
            return regex.match(text).hasMatch();
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
    } else if (mViewDataType == ViewDataType::SymbolView || mViewDataType == ViewDataType::Jaccobian) {
        search(Qt::Horizontal, result);
        search(Qt::Vertical, result);
    } else if (mViewDataType == ViewDataType::EqnAttributes || mViewDataType == ViewDataType::VarAttributes) {
        searchAttributeHeader(Qt::Horizontal, result);
        searchAttributeHeader(Qt::Vertical, result);
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
    auto sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                             : mModelInstance->equation(sectionIndex);
    if (compare(sym->name())) {
        result.append(SearchResult{logicalIndex, orientation});
    } else {
        auto labels = sym->sectionLabels()[sectionIndex];
        Q_FOREACH(const auto label, labels) {
            if (compare(label)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

void Search::searchAttributeHeader(Qt::Orientation orientation, QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        auto value = mDataModel->headerData(section, orientation);
        if (mViewDataType == ViewDataType::EqnAttributes &&
                orientation == Qt::Horizontal &&
                compare(value.toString())) {
            result.append(SearchResult{section, orientation});
            continue;
        } else if (mViewDataType == ViewDataType::VarAttributes &&
                   orientation == Qt::Vertical &&
                   compare(value.toString())) {
            result.append(SearchResult{section, orientation});
            continue;
        }
        int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        if (!ok) continue;
        searchHeader(section, realSection, orientation, result);
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
    Symbol *sym;
    if (mAppliedAggregation.indexToSymbol(orientation).contains(sectionIndex)) {
        sym = mAppliedAggregation.indexToSymbol(orientation).value(sectionIndex);
    } else {
        sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                            : mModelInstance->equation(sectionIndex);
    }
    if (orientation == Qt::Horizontal && compare(sym->name())) {
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
    if (sectionIndex < constant->PredefinedHeaderLength && compare(constant->PredefinedHeader.at(sectionIndex))) {
        result.append(SearchResult{logicalIndex, orientation});
        return;
    }

    Symbol *sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                                : mModelInstance->equation(sectionIndex);
    if (!mAppliedAggregation.indexToSymbol(orientation).contains(sectionIndex) &&
            !mAppliedAggregation.indexToSymbol(orientation).contains(sym->firstSection())) {
        searchHeader(logicalIndex, sectionIndex, orientation, result);
        return;
    }
    auto item = mAppliedAggregation.aggregationSymbols(orientation).value(sym->firstSection());
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
