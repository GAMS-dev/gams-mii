#include "abstracttableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "abstractmodelinstance.h"
#include "search.h"
#include "viewconfigurationprovider.h"

namespace gams {
namespace studio{
namespace modelinspector {

AbstractTableViewFrame::AbstractTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f)
    , ui(new Ui::StandardTableViewFrame)
{
    ui->setupUi(this);
}

AbstractTableViewFrame::~AbstractTableViewFrame()
{
    delete ui;
}

const IdentifierFilter &AbstractTableViewFrame::identifierFilter() const
{
    return mViewConfig->currentIdentifierFilter();
}

const IdentifierFilter &AbstractTableViewFrame::defaultIdentifierFilter() const
{
    return mViewConfig->defaultIdentifierFilter();
}

void AbstractTableViewFrame::setDefaultIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setDefaultIdentifierFilter(filter);
}

const ValueFilter &AbstractTableViewFrame::valueFilter() const
{
    return mViewConfig->currentValueFilter();
}

const ValueFilter &AbstractTableViewFrame::defaultValueFilter() const
{
    return mViewConfig->defaultValueFilter();
}

void AbstractTableViewFrame::setValueFilter(const ValueFilter &filter)
{
    mViewConfig->setCurrentValueFilter(filter);
}

void AbstractTableViewFrame::setDefaultValueFilter(const ValueFilter &filter)
{
    return mViewConfig->setDefaultValueFilter(filter);
}

const LabelFilter &AbstractTableViewFrame::labelFilter() const
{
    return mViewConfig->currentLabelFiler();
}

const LabelFilter &AbstractTableViewFrame::defaultLabelFilter() const
{
    return mViewConfig->defaultLabelFilter();
}

void AbstractTableViewFrame::setLabelFilter(const LabelFilter &filter)
{
    mViewConfig->setCurrentLabelFilter(filter);
}

void AbstractTableViewFrame::setDefaultLabelFilter(const LabelFilter &filter)
{
    mViewConfig->setDefaultLabelFilter(filter);
}

const Aggregation &AbstractTableViewFrame::currentAggregation() const
{
    return mViewConfig->currentAggregation();
}

const Aggregation &AbstractTableViewFrame::defaultAggregation() const
{
    return mViewConfig->defaultAggregation();
}

void AbstractTableViewFrame::setDefaultAggregation(const Aggregation &aggregation)
{
    mViewConfig->setDefaultAggregation(aggregation);
}

ViewDataType AbstractTableViewFrame::type() const
{
    return ViewDataType::Unknown;
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

int AbstractTableViewFrame::view() const
{
    return mViewConfig->view();
}

void AbstractTableViewFrame::setView(int view)
{
    mViewConfig->setView(view);
}

QSharedPointer<AbstractViewConfiguration> AbstractTableViewFrame::viewConfig() const
{
    return mViewConfig;
}

void AbstractTableViewFrame::setViewConfig(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    mViewConfig = viewConfig;
}

QList<SearchResult> AbstractTableViewFrame::search(const QString &term, bool isRegEx)
{
    QList<SearchResult> result;
    if (!term.isEmpty()) {
        Search search(mModelInstance, mViewConfig, ui->tableView->model(), term, isRegEx);
        search.run(result);
    }
    return result;
}

void AbstractTableViewFrame::zoomIn()
{
    ui->tableView->zoomIn(constant->ZoomFactor);
}

void AbstractTableViewFrame::zoomOut()
{
    ui->tableView->zoomOut(constant->ZoomFactor);
}

void AbstractTableViewFrame::resetZoom()
{
    ui->tableView->resetZoom();
}

}
}
}
