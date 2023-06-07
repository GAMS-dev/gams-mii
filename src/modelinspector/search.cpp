#include "search.h"
#include "common.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"

#include <QAbstractItemModel>
#include <QRegularExpression>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

Search::Search(QSharedPointer<AbstractModelInstance> modelInstance,
               QSharedPointer<AbstractViewConfiguration> viewConfig,
               QAbstractItemModel *dataModel,
               const QString &term,
               bool isRegEx)
    : mModelInstance(modelInstance)
    , mViewConfig(viewConfig)
    , mDataModel(dataModel)
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
    if (!mModelInstance || !mDataModel)
        return;
    switch (mViewConfig->viewType()) {
    case ViewDataType::BP_Overview:
        searchPlainHeader(Qt::Horizontal, result);
        searchPlainHeader(Qt::Vertical, result);
        break;
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
    case ViewDataType::BP_Scaling:
        searchPlainHeader(Qt::Horizontal, result);
        searchHeaderHierarchy(Qt::Vertical, result);
        break;
    default:
        searchHeaderHierarchy(Qt::Horizontal, result);
        searchHeaderHierarchy(Qt::Vertical, result);
        break;
    }
}

void Search::searchPlainHeader(Qt::Orientation orientation, QList<SearchResult> &result)
{
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        auto label = mDataModel->headerData(section, orientation, Mi::LabelDataRole).toString();
        if (compare(label)) result.append(SearchResult{section, orientation});
    }
}

void Search::searchHeaderHierarchy(Qt::Orientation orientation, QList<SearchResult> &result)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();

    switch (mViewConfig->viewType()) {
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
    case ViewDataType::BP_Scaling:
        //for (int section=0; section<sections; ++section) {
        //    int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        //    if (!ok) continue;
        //    searchFixedHeaderHierarchy(section, realSection, orientation, result);
        //}
        break;
    default:
        for (int section=0; section<sections; ++section) {
            int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
            if (!ok) continue;
            searchHeaderHierarchy(section, realSection, orientation, result);
        }
        break;
    }
}

void Search::searchHeaderHierarchy(int logicalIndex, int sectionIndex,
                                   Qt::Orientation orientation, QList<SearchResult> &result)
{
    auto sym = orientation == Qt::Horizontal ? mModelInstance->variable(sectionIndex)
                                             : mModelInstance->equation(sectionIndex);
    if (compare(sym->name())) {
        result.append(SearchResult{logicalIndex, orientation});
    } else {
        auto labels = sym->sectionLabels()[sectionIndex];
        for (const auto& label : labels) {
            if (compare(label)) {
                result.append(SearchResult{logicalIndex, orientation});
                break;
            }
        }
    }
}

}
}
}
