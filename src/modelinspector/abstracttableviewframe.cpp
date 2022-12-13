#include "abstracttableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "abstractmodelinstance.h"
#include "search.h"

namespace gams {
namespace studio{
namespace modelinspector {

AbstractTableViewFrame::AbstractTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
    , ui(new Ui::StandardTableViewFrame)
{
    ui->setupUi(this);
}

const IdentifierFilter &AbstractTableViewFrame::identifierFilter() const
{
    return mCurrentIdentifierFilter;
}

const IdentifierFilter &AbstractTableViewFrame::defaultIdentifierFilter() const
{
    return mDefaultIdentifierFilter;
}

const ValueFilter &AbstractTableViewFrame::valueFilter() const
{
    return mCurrentValueFilter;
}

const ValueFilter &AbstractTableViewFrame::defaultValueFilter() const
{
    return mDefaultValueFilter;
}

void AbstractTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    mCurrentValueFilter = filter;
}

const LabelFilter &AbstractTableViewFrame::labelFilter() const
{
    return mCurrentLabelFilter;
}

const LabelFilter &AbstractTableViewFrame::defaultLabelFilter() const
{
    return mDefaultLabelFilter;
}

void AbstractTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    mCurrentLabelFilter = filter;
}

const Aggregation &AbstractTableViewFrame::aggregation() const
{
    return mCurrentAggregation;
}

const Aggregation &AbstractTableViewFrame::defaultAggregation() const
{
    return mDefaultAggregation;
}

PredefinedViewEnum AbstractTableViewFrame::type() const
{
    return PredefinedViewEnum::Unknown;
}

QAbstractItemModel *AbstractTableViewFrame::model() const
{
    return ui->tableView->model();
}

void AbstractTableViewFrame::setSearchSelection(const gams::studio::modelinspector::SearchResult &result)
{
    if (result.Index < 0) return;
    if (result.Orientation == Qt::Horizontal) {
        ui->tableView->selectColumn(result.Index);
    } else {
        ui->tableView->selectRow(result.Index);
    }
}

QList<SearchResult> AbstractTableViewFrame::search(const QString &term, bool isRegEx)
{
    QList<SearchResult> result;
    if (!term.isEmpty()) {
        Search search(mModelInstance, ui->tableView->model(),
                      appliedAggregation(), term, isRegEx);
        search.run(result);
    }
    return result;
}

void AbstractTableViewFrame::zoomIn()
{
    ui->tableView->zoomIn(ZoomFactor);
}

void AbstractTableViewFrame::zoomOut()
{
    ui->tableView->zoomOut(ZoomFactor);
}

void AbstractTableViewFrame::resetZoom()
{
    ui->tableView->resetZoom();
}

IdentifierStates AbstractTableViewFrame::defaultSymbolFilter(QAbstractItemModel *model,
                                                             Qt::Orientation orientation) const
{
    int sections = orientation == Qt::Horizontal ? model->columnCount() :
                                                   model->rowCount();
    bool ok;
    IdentifierStates states;
    QSet<QString> symNames;
    for (int s=0; s<sections; ++s) {
        int realSection = model->headerData(s, orientation).toInt(&ok);
        if (!ok) continue;
        auto data = mModelInstance->headerData(realSection, -1, orientation);
        if (realSection < PredefinedHeaderLength) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        } else if (!symNames.contains(data)) {
            symNames.insert(data);
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SymbolIndex = realSection;
            identifierState.Text = data;
            identifierState.Checked = Qt::Checked;
            states[realSection] = identifierState;
        }
    }
    return states;
}

Aggregation AbstractTableViewFrame::appliedAggregation(const Aggregation &aggregation, int view) const
{
    AggregationMap map;
    Aggregation appliedAggregation;
    for (auto mapIter=aggregation.aggregationMap().keyValueBegin(); mapIter!=aggregation.aggregationMap().keyValueEnd(); ++mapIter) {
        if (mapIter->first == Qt::Horizontal) {
            int lastSymEndIndex = -1;
            Q_FOREACH(auto item, mapIter->second) {
                for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                    if (iter->second != Qt::Unchecked) {
                        auto symbol = mModelInstance->variable(item.symbolIndex());
                        Aggregator aggregator(symbol);
                        aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                     labelFilter().LabelCheckStates.value(mapIter->first),
                                                     labelFilter().Any);
                        aggregator.aggregate(item, aggregation.type(), aggregation.typeText(), lastSymEndIndex);
                        appliedAggregation.indexToSymbol(Qt::Horizontal)[item.symbolIndex()] = symbol;
                        map[mapIter->first][item.symbolIndex()] = item;
                        break;
                    }
                }
            }
        } else {
            int lastSymEndIndex = -1;
            Q_FOREACH(auto item, mapIter->second) {
                for (auto iter=item.checkStates().keyValueBegin(); iter!=item.checkStates().keyValueEnd(); ++iter) {
                    if (iter->second != Qt::Unchecked) {
                        auto symbol = mModelInstance->equation(item.symbolIndex());
                        Aggregator aggregator(symbol);
                        aggregator.applyFilterStates(identifierFilter().value(mapIter->first)[symbol.firstSection()],
                                                     labelFilter().LabelCheckStates.value(mapIter->first),
                                                     labelFilter().Any);
                        aggregator.aggregate(item, aggregation.type(), aggregation.typeText(), lastSymEndIndex);
                        appliedAggregation.indexToSymbol(Qt::Vertical)[item.symbolIndex()] = symbol;
                        map[mapIter->first][item.symbolIndex()] = item;
                        break;
                    }
                }
            }
        }
    }

    appliedAggregation.setUseAbsoluteValues(aggregation.useAbsoluteValues());
    appliedAggregation.setType(aggregation.type());
    appliedAggregation.setAggregationMap(map);
    appliedAggregation.setIdentifierFilter(identifierFilter());
    appliedAggregation.setValueFilter(valueFilter());
    appliedAggregation.setViewType(type());
    appliedAggregation.setView(view);
    return appliedAggregation;
}

Aggregation AbstractTableViewFrame::getDefaultAggregation() const
{
    auto initAggregation = [this](Symbol::Type type) {
        AggregationSymbols items;
        Q_FOREACH(const auto& sym, mModelInstance->symbols(type)) {
            AggregationItem aggrItem;
            aggrItem.setText(sym.name());
            aggrItem.setSymbolIndex(sym.firstSection());
            aggrItem.setDomainLabels(sym.domainLabels());
            for (int d=1; d<=sym.dimension(); ++d) {
                aggrItem.setCheckState(d, Qt::Unchecked);
            }
            items[aggrItem.symbolIndex()] = aggrItem;
        }
        return items;
    };
    Aggregation aggregation;
    aggregation.setViewType(type());
    aggregation.setAggregationSymbols(Qt::Horizontal, initAggregation(Symbol::Variable));
    aggregation.setAggregationSymbols(Qt::Vertical, initAggregation(Symbol::Equation));
    return aggregation;
}

}
}
}
