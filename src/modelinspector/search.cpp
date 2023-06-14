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

Search::Search(QSharedPointer<AbstractViewConfiguration> viewConfig,
               QAbstractItemModel *dataModel,
               const QString &term,
               bool isRegEx)
    : mViewConfig(viewConfig)
    , mDataModel(dataModel)
{
    mViewConfig->searchResult().Term = term;
    mViewConfig->searchResult().IsRegEx = isRegEx;
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

void Search::run()
{
    if (!mDataModel)
        return;
    switch (mViewConfig->viewType()) {
    case ViewDataType::BP_Overview:
    case ViewDataType::BP_Count:
    case ViewDataType::BP_Average:
    case ViewDataType::BP_Scaling:
        searchStaticHeader(Qt::Horizontal);
        searchStaticHeader(Qt::Vertical);
        break;
    default:
        searchHeaderHierarchy(Qt::Horizontal);
        searchHeaderHierarchy(Qt::Vertical);
        break;
    }
}

void Search::searchStaticHeader(Qt::Orientation orientation)
{
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount()
                                                 : mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        auto labels = mDataModel->headerData(section, orientation, Mi::SectionLabelRole).toStringList();
        for (const auto& label : labels) {
            if (compare(label)) {
                mViewConfig->searchResult().Entries.append(SearchResult::SearchEntry{section, orientation});
                break;
            }
        }
    }
}

void Search::searchHeaderHierarchy(Qt::Orientation orientation)
{
    bool ok;
    int sections = orientation == Qt::Horizontal ? mDataModel->columnCount() :
                                                   mDataModel->rowCount();
    for (int section=0; section<sections; ++section) {
        int realSection = mDataModel->headerData(section, orientation).toInt(&ok);
        if (!ok) continue;
        searchHeaderHierarchy(section, realSection, orientation);
    }
}

void Search::searchHeaderHierarchy(int logicalIndex, int sectionIndex, Qt::Orientation orientation)
{
    auto sym = orientation == Qt::Horizontal ? mViewConfig->modelInstance()->variable(sectionIndex)
                                             : mViewConfig->modelInstance()->equation(sectionIndex);
    if (compare(sym->name())) {
        mViewConfig->searchResult().Entries.append(SearchResult::SearchEntry{logicalIndex, orientation});
    } else {
        auto labels = sym->sectionLabels()[sectionIndex];
        for (const auto& label : labels) {
            if (compare(label)) {
                mViewConfig->searchResult().Entries.append(SearchResult::SearchEntry{logicalIndex, orientation});
                break;
            }
        }
    }
}

}
}
}
