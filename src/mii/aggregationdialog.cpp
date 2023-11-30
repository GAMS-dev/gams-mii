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
#include "aggregationdialog.h"
#include "ui_aggregationdialog.h"
#include "abstractmodelinstance.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"
#include "viewconfigurationprovider.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace mii {

const QRegularExpression AggregationDialog::RegExp = QRegularExpression("^\\d+$");

AggregationDialog::AggregationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AggregationDialog)
    , mViewConfig(ViewConfigurationProvider::configuration(ViewHelper::ViewDataType::Unknown,
                                                           QSharedPointer<AbstractModelInstance>(new EmptyModelInstance)))
{
    ui->setupUi(this);
    connect(ui->filterEdit, &QLineEdit::textChanged,
            this, &AggregationDialog::filterUpdate);
    connect(this, &QDialog::rejected,
            this, &AggregationDialog::on_cancelButton_clicked);
}

AggregationDialog::~AggregationDialog()
{
    delete ui;
}

const QSharedPointer<AbstractViewConfiguration> &AggregationDialog::viewConfig() const
{
    return mViewConfig;
}

void AggregationDialog::setViewConfig(const QSharedPointer<AbstractViewConfiguration> &config)
{
    mViewConfig = config;
    mAbsValuesGlobal = mViewConfig->currentAggregation().useAbsoluteValues();
    ui->filterEdit->setText("");
    ui->aggregationBox->setCurrentText(mViewConfig->currentAggregation().typeText());
    mAggregationMethod = ui->aggregationBox->currentIndex();
    ui->absoluteBox->setChecked(mViewConfig->currentAggregation().useAbsoluteValues());
    if (ViewHelper::isAggregatable(mViewConfig->currentAggregation().viewType())) {
        ui->applyButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
    } else {
        ui->applyButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
    }
    setupAggregationView();
    if (mAbsValuesGlobal)
        ui->absoluteBox->setEnabled(false);
    else
        ui->absoluteBox->setEnabled(true);
}

void AggregationDialog::on_selectButton_clicked()
{
    applyCheckState(true);
}

void AggregationDialog::on_deselectButton_clicked()
{
    applyCheckState(false);
}

void AggregationDialog::on_cancelButton_clicked()
{
    //setAggregation(mAggregation, mIdentifierFilter, mAbsValuesGlobal);
    close();
}

void AggregationDialog::on_resetButton_clicked()
{
    //setAggregation(mDefaultAggregation, mIdentifierFilter, mAbsValuesGlobal);
    applyAggregation();
    emit aggregationUpdated();
}

void AggregationDialog::on_applyButton_clicked()
{
    if (ui->aggregationBox->currentIndex() == 0)
        return;
    auto item = static_cast<FilterTreeModel*>(mAggregationModel->sourceModel())->filterItem();
    if (item->checked() == Qt::Unchecked)
        return;
    applyAggregation();
    mAggregationMethod = ui->aggregationBox->currentIndex();
    emit aggregationUpdated();
}

void AggregationDialog::filterUpdate(const QString &text)
{
    if (!mAggregationModel) return;
    if (RegExp.match(text).hasMatch() || text.isEmpty()) {
        mAggregationModel->setFilterWildcard(text);
        ui->view->expandAll();
    }
}

void AggregationDialog::setupAggregationView()
{
    auto rootItem = new FilterTreeItem;
    rootItem->setCheckable(false);
    setupTreeItems(Qt::Vertical, rootItem);
    setupTreeItems(Qt::Horizontal, rootItem);

    auto oldVarModel = ui->view->selectionModel();
    auto treeModel = new FilterTreeModel(rootItem, ui->view);
    mAggregationModel = new AggregationTreeItemFilterProxyModel(ui->view);
    mAggregationModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mAggregationModel->setRecursiveFilteringEnabled(true);
    mAggregationModel->setSourceModel(treeModel);
    ui->view->setModel(mAggregationModel);
    ui->view->expandAll();
    if (oldVarModel)
        oldVarModel->deleteLater();
    connect(ui->filterEdit, &QLineEdit::textChanged, this,
            [this](const QString &text) {
                                            if (!mAggregationModel) return;
                                            mAggregationModel->setFilterWildcard(text);
                                            ui->view->expandAll();
                                        });
}

void AggregationDialog::setupTreeItems(Qt::Orientation orientation,
                                       FilterTreeItem *root)
{
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ? ViewHelper::VariableHeaderText :
                                                                       ViewHelper::EquationHeaderText,
                                       Qt::Unchecked, root);
    typeItem->setCheckable(false);
    auto identifierStates = mViewConfig->currentIdentifierFilter()[orientation];
    Q_FOREACH(const auto& item, mViewConfig->currentAggregation().aggregationSymbols(orientation)) {
        if (item.checkStates().isEmpty())
            continue;
        if (identifierStates[item.symbolIndex()].Checked == Qt::Unchecked)
            continue;
        auto sItem = new FilterTreeItem(item.text(), Qt::Unchecked, typeItem);
        sItem->setVisible(!(item.checkStates().size() == 1 && item.checkStates().contains(0)));
        sItem->setSymbolIndex(item.symbolIndex());
        sItem->setCheckable(false);
        typeItem->append(sItem);
        for (auto iter=item.checkStates().keyValueBegin();
             iter!=item.checkStates().constKeyValueEnd(); ++iter)
        {
            auto text = QString::number(iter->first);
            if (iter->first) {
                text += " - " + item.domainLabel(iter->first-1);
            }
            auto dItem = new FilterTreeItem(text, iter->second, sItem);
            dItem->setVisible(sItem->isVisible());
            sItem->setSymbolIndex(item.symbolIndex());
            sItem->append(dItem);
        }
    }
    typeItem->hasChildren() ? root->append(typeItem) : delete typeItem;
}

void AggregationDialog::applyAggregation()
{
    mViewConfig->currentAggregation().setUseAbsoluteValues(ui->absoluteBox->isChecked());
    mViewConfig->currentAggregation().setType(ui->aggregationBox->currentText());
    QList<FilterTreeItem*> items {
        static_cast<FilterTreeModel*>(mAggregationModel->sourceModel())->filterItem()->childs()
    };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        if (item->text() == ViewHelper::EquationHeaderText) {
            mViewConfig->currentAggregation().setAggregationSymbols(Qt::Vertical, checkStates(item));
        } else if (item->text() == ViewHelper::VariableHeaderText) {
            mViewConfig->currentAggregation().setAggregationSymbols(Qt::Horizontal, checkStates(item));
        }
    }
}

AggregationSymbols AggregationDialog::checkStates(FilterTreeItem *item)
{
    QList<FilterTreeItem*> items { item->childs() };
    QMap<int, AggregationItem> states;
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        AggregationItem aggrItem;
        aggrItem.setText(item->text());
        aggrItem.setSymbolIndex(item->symbolIndex());
        DomainLabels domainLabels;
        for (int d=0; d<item->childs().size(); ++d) {
            auto child = item->child(d);
            if (!child) continue;
            aggrItem.setCheckState(d+1, item->child(d)->checked());
            domainLabels.push_back(child->text().split("-").last().trimmed());
        }
        aggrItem.setDomainLabels(domainLabels);
        states[aggrItem.symbolIndex()] = aggrItem;
    }
    return states;
}

void AggregationDialog::applyCheckState(bool state)
{
    QModelIndexList indexes;
    for(int row=0; row<mAggregationModel->rowCount(); ++row) {
        auto index = mAggregationModel->index(row, 0);
        if (ui->view->isExpanded(index) && !ui->view->isRowHidden(row, QModelIndex()))
            indexes.append(index);
    }
    while (!indexes.isEmpty()) {
        auto index = indexes.takeFirst();
        if (!mAggregationModel->hasChildren(index))
            mAggregationModel->setData(index, state, Qt::CheckStateRole);
        if (mAggregationModel->hasChildren(index)) {
            for(int row=0; row<mAggregationModel->rowCount(index); ++row)
                indexes.append(mAggregationModel->index(row, 0, index));
        }
    }
}

}
}
}
