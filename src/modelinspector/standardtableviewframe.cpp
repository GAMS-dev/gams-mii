/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "standardtableviewframe.h"
#include "ui_standardtableviewframe.h"
#include "hierarchicalheaderview.h"
#include "identifierfiltermodel.h"
#include "labelfiltermodel.h"
#include "valueformatproxymodel.h"
#include "abstractmodelinstance.h"
#include "viewconfigurationprovider.h"

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

AbstractStandardTableViewFrame::AbstractStandardTableViewFrame(QWidget *parent, Qt::WindowFlags f)
    : AbstractTableViewFrame(parent, f)
{

}

void AbstractStandardTableViewFrame::setIdentifierFilter(const IdentifierFilter &filter)
{
    mViewConfig->setCurrentIdentifierFilter(filter);
    if (mIdentifierFilterModel) mIdentifierFilterModel->setIdentifierFilter(filter);
}

void AbstractStandardTableViewFrame::setAggregation(const Aggregation &aggregation)
{
    Q_UNUSED(aggregation);
}

void AbstractStandardTableViewFrame::setShowAbsoluteValues(bool absoluteValues)
{
    Q_UNUSED(absoluteValues);
}

void AbstractStandardTableViewFrame::reset()
{
    setIdentifierFilter(mViewConfig->defaultIdentifierFilter());
    setValueFilter(mViewConfig->defaultValueFilter());
    setLabelFilter(mViewConfig->defaultLabelFilter());
    setAggregation(mViewConfig->defaultAggregation());
    updateView();
}

void AbstractStandardTableViewFrame::updateView()
{
    //ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    emit filtersChanged();
}

void AbstractStandardTableViewFrame::setIdentifierFilterCheckState(int symbolIndex,
                                                                   Qt::CheckState state,
                                                                   Qt::Orientation orientation)
{
    auto symbols = mViewConfig->currentIdentifierFilter()[orientation];
    for (auto iter=symbols.begin(); iter!=symbols.end(); ++iter) {
        if (iter->SymbolIndex == symbolIndex) {
            iter->Checked = state;
            break;
        }
    }
    mViewConfig->currentIdentifierFilter()[orientation] = symbols;
}

}
}
}
