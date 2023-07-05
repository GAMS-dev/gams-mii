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
#include "labelfilterwidget.h"
#include "ui_labelfilterwidget.h"
#include "filtertreemodel.h"
#include "filtertreeitem.h"

#include <QSortFilterProxyModel>
#include <QMenu>

namespace gams {
namespace studio {
namespace modelinspector {

LabelFilterWidget::LabelFilterWidget(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LabelFilterWidget)
    , mOrientation(orientation)
{
    ui->setupUi(this);
}

LabelFilterWidget::~LabelFilterWidget()
{
    delete ui;
}

void LabelFilterWidget::setData(FilterTreeItem *rootItem)
{
    auto oldModel = ui->labelView->selectionModel();
    auto treeModel = new FilterTreeModel(rootItem, ui->labelView);
    mFilterModel = new QSortFilterProxyModel(ui->labelView);
    mFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mFilterModel->setRecursiveFilteringEnabled(true);
    mFilterModel->setSourceModel(treeModel);
    ui->labelView->setModel(mFilterModel);
    ui->labelView->expandAll();
    if (oldModel)
        oldModel->deleteLater();
    connect(ui->labelEdit, &QLineEdit::textChanged,
            this, &LabelFilterWidget::applyFilter);
}

void LabelFilterWidget::showEvent(QShowEvent *event)
{
    applyFilter(ui->labelEdit->text());
    QWidget::showEvent(event);
}

void LabelFilterWidget::on_applyButton_clicked()
{
    emit filterChanged(identifierState(), mOrientation);
    static_cast<QMenu*>(this->parent())->close();
    ui->labelEdit->clear();
}

void LabelFilterWidget::on_selectButton_clicked()
{
    applyCheckState(true);
}

void LabelFilterWidget::on_deselectButton_clicked()
{
    applyCheckState(false);
}

void LabelFilterWidget::applyFilter(const QString &text)
{
    mFilterModel->setFilterWildcard(text);
    ui->labelView->expandAll();
}

void LabelFilterWidget::applyCheckState(bool state)
{
    QModelIndexList indexes;
    for(int row=0; row<mFilterModel->rowCount(); ++row) {
        indexes.append(mFilterModel->index(row, 0));
    }
    while (!indexes.isEmpty()) {
        auto index = indexes.takeFirst();
        if (mFilterModel->flags(index) == Qt::NoItemFlags)
            continue;
        if (!mFilterModel->hasChildren(index))
            mFilterModel->setData(index, state, Qt::CheckStateRole);
        if (mFilterModel->hasChildren(index)) {
            for(int row=0; row<mFilterModel->rowCount(index); ++row)
                indexes.append(mFilterModel->index(row, 0, index));
        }
    }
}

IdentifierState LabelFilterWidget::identifierState()
{
    QList<FilterTreeItem*> items {
        static_cast<FilterTreeModel*>(mFilterModel->sourceModel())->filterItem()
    };
    IdentifierState state;
    state.Enabled = true;
    state.SectionIndex = items.first()->sectionIndex();
    state.SymbolIndex = items.first()->symbolIndex();
    state.Text = items.first()->text();
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        items.append(item->childs());
        if (!item->isCheckable())
            continue;
        state.CheckStates[item->sectionIndex()] = item->checked();
    }
    state.Checked = state.disabled() ? Qt::Unchecked : Qt::Checked;
    return state;
}

} // namespace modelinspector
} // namespace studio
} // namespace gams
