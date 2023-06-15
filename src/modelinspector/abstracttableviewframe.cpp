#include "abstracttableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "search.h"
#include "viewconfigurationprovider.h"

namespace gams {
namespace studio{
namespace modelinspector {

AbstractTableViewFrame::AbstractTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractViewFrame(parent, f)
    , ui(new Ui::StandardTableViewFrame)
{
    ui->setupUi(this);
}

AbstractTableViewFrame::~AbstractTableViewFrame()
{
    delete ui;
}

void AbstractTableViewFrame::setSearchSelection(const SearchResult::SearchEntry &result)
{
    if (result.Index < 0) return;
    if (result.Orientation == Qt::Horizontal) {
        ui->tableView->selectColumn(result.Index);
    } else {
        ui->tableView->selectRow(result.Index);
    }
}

SearchResult& AbstractTableViewFrame::search(const QString &term, bool isRegEx)
{
    if (!mViewConfig->searchResult().Entries.isEmpty()) {
        mViewConfig->searchResult().Entries.clear();
    }
    if (!term.isEmpty()) {
        Search search(mViewConfig, ui->tableView->model(), term, isRegEx);
        search.run();
    }
    return mViewConfig->searchResult();
}

void AbstractTableViewFrame::zoomIn()
{
    ui->tableView->zoomIn(Mi::ZoomFactor);
}

void AbstractTableViewFrame::zoomOut()
{
    ui->tableView->zoomOut(Mi::ZoomFactor);
}

void AbstractTableViewFrame::resetZoom()
{
    ui->tableView->resetZoom();
}

void AbstractTableViewFrame::setIdentifierLabelFilter(const IdentifierState &state,
                                                      Qt::Orientation orientation)
{
    Q_UNUSED(state);
    Q_UNUSED(orientation);
}

}
}
}
