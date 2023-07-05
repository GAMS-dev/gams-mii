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
#include "filterdialog.h"
#include "ui_filterdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>

namespace gams {
namespace studio {
namespace modelinspector {

// TODO (AF) consolidate filters, aggregation, row/column data source... struct/class?

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
    ui->labelView->sortByColumn(0, Qt::AscendingOrder);
    ui->minEdit->setValidator(new QDoubleValidator(ui->minEdit));
    ui->maxEdit->setValidator(new QDoubleValidator(ui->maxEdit));

    connect(this, &QDialog::rejected,
            this, &FilterDialog::on_cancelButton_clicked);
    connect(ui->absoluteBox, &QCheckBox::stateChanged,
            this, [this](){
        updateRangeEdit(ui->minEdit, ui->minEdit->text());
        updateRangeEdit(ui->maxEdit, ui->maxEdit->text());
    });
    connect(ui->minEdit, &QLineEdit::textEdited,
            this, [this](const QString &text) {
        mValueFilter.setIsUserInput(true);
        updateRangeEdit(ui->minEdit, text);
    });
    connect(ui->maxEdit, &QLineEdit::textEdited,
            this, [this](const QString &text) {
        mValueFilter.setIsUserInput(true);
        updateRangeEdit(ui->maxEdit, text);
    });
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

IdentifierFilter FilterDialog::idendifierFilter() const
{
    return mIdentifierFilter;
}

void FilterDialog::setIdentifierFilter(const IdentifierFilter &filter)
{
    mIdentifierFilter = filter;
    setupEquationFilter(mIdentifierFilter[equationOrientation()], mDefaultIdentifierFilter[equationOrientation()]);
    setupVariableFilter(mIdentifierFilter[variableOrientation()], mDefaultIdentifierFilter[variableOrientation()]);
}

void FilterDialog::setDefaultIdentifierFilter(const IdentifierFilter &filter)
{
    mDefaultIdentifierFilter = filter;
}

ValueFilter FilterDialog::valueFilter() const
{
    return mValueFilter;
}

void FilterDialog::setValueFilter(const ValueFilter &filter)
{
    mValueFilter = filter;
    ui->minEdit->setText(QString::number(mValueFilter.MinValue));
    updateRangeEdit(ui->minEdit, QString::number(mValueFilter.MinValue));
    ui->maxEdit->setText(QString::number(mValueFilter.MaxValue));
    updateRangeEdit(ui->maxEdit, QString::number(mValueFilter.MinValue));
    ui->excludeBox->setChecked(mValueFilter.ExcludeRange);
    ui->absoluteBox->setChecked(mValueFilter.UseAbsoluteValues);
    if (mValueFilter.UseAbsoluteValuesGlobal)
        ui->absoluteBox->setEnabled(false);
    else
        ui->absoluteBox->setEnabled(true);
    ui->epsBox->setChecked(mValueFilter.ShowEps);
    ui->nInfBox->setChecked(mValueFilter.ShowNInf);
    ui->pInfBox->setChecked(mValueFilter.ShowPInf);
}

void FilterDialog::setDefaultValueFilter(const ValueFilter &filter)
{
    mDefaultValueFilter = filter;
}

LabelFilter FilterDialog::labelFilter() const
{
    return mLabelFilter;
}

void FilterDialog::setLabelFilter(const LabelFilter &filter)
{
    mLabelFilter = filter;
    setupLabelFilter();
}

void FilterDialog::setDefaultLabelFilter(const LabelFilter &filter)
{
    mDefaultLabelFilter = filter;
}

ViewDataType FilterDialog::viewType() const
{
    return mViewType;
}

void FilterDialog::setViewType(ViewDataType viewType)
{
    mViewType = viewType;
}

void FilterDialog::on_applyButton_clicked()
{
    mIdentifierFilter[variableOrientation()] = applyHeaderFilter(mVarFilterModel);
    mIdentifierFilter[equationOrientation()] = applyHeaderFilter(mEqnFilterModel);
    applyValueFilter();
    mLabelFilter.LabelCheckStates[variableOrientation()] = applyLabelFilter(variableOrientation(), mLabelFilterModel);
    mLabelFilter.LabelCheckStates[equationOrientation()] = applyLabelFilter(equationOrientation(), mLabelFilterModel);
    mLabelFilter.Any = ui->labelBox->currentIndex();
    emit filterUpdated();
}

void FilterDialog::on_resetButton_clicked()
{
    setIdentifierFilter(mDefaultIdentifierFilter);
    resetValueFilter();
    setLabelFilter(mDefaultLabelFilter);

    ui->rowEdit->setText("");
    ui->columnEdit->setText("");
    ui->labelEdit->setText("");
    ui->labelBox->setCurrentIndex(0);

    on_applyButton_clicked();
}

void FilterDialog::on_cancelButton_clicked()
{
    setIdentifierFilter(mIdentifierFilter);
    setValueFilter(mValueFilter);
    setLabelFilter(mLabelFilter);

    ui->rowEdit->setText("");
    ui->columnEdit->setText("");
    ui->labelEdit->setText("");
    ui->labelBox->setCurrentIndex(0);

    close();
}

void FilterDialog::on_selectEqnButton_clicked()
{
    applyCheckState(ui->rowView, mEqnFilterModel, Qt::Checked);
    ui->rowView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_deselectEqnButton_clicked()
{
    applyCheckState(ui->rowView, mEqnFilterModel, Qt::Unchecked);
    ui->rowView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_selectVarButton_clicked()
{
    applyCheckState(ui->columnView, mVarFilterModel, Qt::Checked);
    ui->columnView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_deselectVarButton_clicked()
{
    applyCheckState(ui->columnView, mVarFilterModel, Qt::Unchecked);
    ui->columnView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_selectLabelButton_clicked()
{
    applyCheckState(ui->labelView, mLabelFilterModel, Qt::Checked);
    ui->labelView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_deselectLabelButton_clicked()
{
    applyCheckState(ui->labelView, mLabelFilterModel, Qt::Unchecked);
    ui->labelView->dataChanged(QModelIndex(), QModelIndex());
}

void FilterDialog::on_labelBox_currentIndexChanged(int index)
{
    switch (index) {
    case 1: // Any
        mLabelFilter.Any = true;
        break;
    default: // All
        mLabelFilter.Any = false;
        break;
    }
}

void FilterDialog::setupEquationFilter(const IdentifierStates &filter, const IdentifierStates &dFilter)
{
    auto filterItem = setupSymTreeItems(FilterTreeItem::EquationText, filter, dFilter);
    auto oldEqnModel = ui->rowView->selectionModel();
    auto eqnTreeModel = new FilterTreeModel(filterItem, ui->rowView);
    mEqnFilterModel = new QSortFilterProxyModel(ui->rowView);
    mEqnFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mEqnFilterModel->setRecursiveFilteringEnabled(true);
    mEqnFilterModel->setSourceModel(eqnTreeModel);
    ui->rowView->setModel(mEqnFilterModel);
    ui->rowView->expandAll();
    if (oldEqnModel)
        oldEqnModel->deleteLater();
    connect(ui->rowEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                            if (!mEqnFilterModel) return;
                            mEqnFilterModel->setFilterWildcard(text);
                            ui->rowView->expandAll();
                        });
}

void FilterDialog::setupVariableFilter(const IdentifierStates &filter, const IdentifierStates &dFilter)
{
    auto filterItem = setupSymTreeItems(FilterTreeItem::VariableText, filter, dFilter);
    auto oldVarModel = ui->columnView->selectionModel();
    auto varTreeModel = new FilterTreeModel(filterItem, ui->columnView);
    mVarFilterModel = new QSortFilterProxyModel(ui->columnEdit);
    mVarFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mVarFilterModel->setRecursiveFilteringEnabled(true);
    mVarFilterModel->setSourceModel(varTreeModel);
    ui->columnView->setModel(mVarFilterModel);
    ui->columnView->expandAll();
    if (oldVarModel)
        oldVarModel->deleteLater();
    connect(ui->columnEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) {
                                                if (!mVarFilterModel) return;
                                                mVarFilterModel->setFilterWildcard(text);
                                                ui->columnView->expandAll();
                                              });
}

void FilterDialog::setupLabelFilter()
{
    ui->labelBox->setCurrentIndex(mLabelFilter.Any);
    auto labelFilterItem = new FilterTreeItem("", Qt::Unchecked);
    labelFilterItem->setCheckable(false);
    setupLabelTreeItems(FilterTreeItem::EquationText,
                        equationOrientation(),
                        labelFilterItem);
    setupLabelTreeItems(FilterTreeItem::VariableText,
                        variableOrientation(),
                        labelFilterItem);

    auto oldLabelModel = ui->labelView->selectionModel();
    auto labelTreeModel = new FilterTreeModel(labelFilterItem, ui->labelView);
    mLabelFilterModel = new QSortFilterProxyModel(ui->labelView);
    mLabelFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mLabelFilterModel->setRecursiveFilteringEnabled(true);
    mLabelFilterModel->setSourceModel(labelTreeModel);
    ui->labelView->setModel(mLabelFilterModel);
    ui->labelView->expandAll();
    if (oldLabelModel)
        oldLabelModel->deleteLater();
    connect(ui->labelEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                                                mLabelFilterModel->setFilterWildcard(text);
                                                ui->labelView->expandAll();
                                              });
}

FilterTreeItem* FilterDialog::setupSymTreeItems(const QString &text,
                                                const IdentifierStates &filter,
                                                const IdentifierStates &dFilter)
{
    auto root = new FilterTreeItem(QString(), Qt::Unchecked);
    root->setCheckable(false);
    auto symbols = new FilterTreeItem(text, Qt::Unchecked, root);
    symbols->setCheckable(false);
    for (const auto& item : filter) {
        auto fItem = new FilterTreeItem(item.Text, item.Checked, symbols);
        auto defaultItem = dFilter.value(item.SymbolIndex);
        if (mViewType == ViewDataType::Symbols       &&
                item.Checked == Qt::Unchecked           &&
                defaultItem.Checked == Qt::Unchecked) {
            fItem->setEnabled(false);
        }
        fItem->setSymbolIndex(item.SymbolIndex);
        symbols->append(fItem);
    }
    root->append(symbols);
    return root;
}

void FilterDialog::setupLabelTreeItems(const QString &text,
                                       Qt::Orientation orientation,
                                       FilterTreeItem *root)
{
    auto typeItem = new FilterTreeItem(text, Qt::Unchecked, root);
    typeItem->setCheckable(false);
    root->append(typeItem);

    auto checkStates = mLabelFilter.LabelCheckStates[orientation];
    for (auto iter=checkStates.constKeyValueBegin();
         iter!=checkStates.constKeyValueEnd(); ++iter) {
        auto treeItem = new FilterTreeItem(iter->first,
                                           iter->second ? Qt::Checked : Qt::Unchecked,
                                           typeItem);
        typeItem->append(treeItem);
    }
}

void FilterDialog::applyCheckState(QTreeView *view,
                                   QSortFilterProxyModel *model,
                                   Qt::CheckState state)
{
    QModelIndexList indexes;
    for(int row=0; row<model->rowCount(); ++row) {
        auto index = model->index(row, 0);
        if (view->isExpanded(index) && !view->isRowHidden(row, QModelIndex()))
            indexes.append(index);
    }
    while (!indexes.isEmpty()) {
        auto index = indexes.takeFirst();
        if (model->flags(index) == Qt::NoItemFlags)
            continue;
        model->setData(index, state, Qt::CheckStateRole);
        if (model->hasChildren(index)) {
            for(int row=0; row<model->rowCount(index); ++row)
                indexes.append(model->index(row, 0, index));
        }
    }
}

IdentifierStates FilterDialog::applyHeaderFilter(QSortFilterProxyModel *model)
{
    QList<FilterTreeItem*> items {
        static_cast<FilterTreeModel*>(model->sourceModel())->filterItem()
    };
    IdentifierStates filter;
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        items.append(item->childs());
        if (!item->isCheckable())
            continue;
        IdentifierState identifierState;
        identifierState.Enabled = item->isEnabled();
        identifierState.SymbolIndex = item->symbolIndex();
        identifierState.Text =  item->text();
        identifierState.Checked = item->checked();
        filter[item->symbolIndex()] = identifierState;
    }
    return filter;
}

void FilterDialog::applyValueFilter()
{
    if (mValueFilter.isUserInput()) {
        mValueFilter.MinValue = ui->minEdit->text().toDouble();
        mValueFilter.MaxValue = ui->maxEdit->text().toDouble();
    } else {
        mValueFilter.MinValue = mDefaultValueFilter.MinValue;
        mValueFilter.MaxValue = mDefaultValueFilter.MaxValue;
    }
    mValueFilter.ExcludeRange = ui->excludeBox->isChecked();
    mValueFilter.UseAbsoluteValues = ui->absoluteBox->isChecked();
    mValueFilter.ShowPInf = ui->pInfBox->isChecked();
    mValueFilter.ShowNInf = ui->nInfBox->isChecked();
    mValueFilter.ShowEps = ui->epsBox->isChecked();
}

LabelCheckStates FilterDialog::applyLabelFilter(Qt::Orientation orientation,
                                                QSortFilterProxyModel *model)
{
    FilterTreeItem* root = nullptr;
    auto childs = static_cast<FilterTreeModel*>(model->sourceModel())->filterItem()->childs();
    Q_FOREACH(auto child, childs) {
        if (orientation == variableOrientation() && child->text() == FilterTreeItem::VariableText) {
            root = child;
            break;
        } else if (orientation == equationOrientation() && child->text() == FilterTreeItem::EquationText) {
            root = child;
            break;
        }
    }

    LabelCheckStates filter;
    if (root) {
        QList<FilterTreeItem*> items { root };
        while (!items.isEmpty()) {
            auto item = items.takeFirst();
            items.append(item->childs());
            if (!item->isCheckable())
                continue;
            filter[item->text()] = item->checked();
        }
    }
    return filter;
}

void FilterDialog::updateRangeEdit(QLineEdit *edit, const QString &text)
{
    if (ui->absoluteBox->isChecked() && text.startsWith("-")) {
        edit->setStyleSheet("color: red");
    } else {
        edit->setStyleSheet("color: "+QApplication::palette().text().color().name());
    }
}

void FilterDialog::resetValueFilter()
{
    auto filter = mDefaultValueFilter;
    if (mValueFilter.UseAbsoluteValuesGlobal) {
        filter.UseAbsoluteValues = mValueFilter.UseAbsoluteValues;
        filter.UseAbsoluteValuesGlobal = mValueFilter.UseAbsoluteValuesGlobal;
    }
    setValueFilter(filter);
}

}
}
}
