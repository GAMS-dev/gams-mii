#include "aggregationdialog.h"
#include "ui_aggregationdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

AggregationDialog::AggregationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AggregationDialog)
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

const Aggregation& AggregationDialog::aggregation() const
{
    return mAggregation;
}

void AggregationDialog::setAggregation(const Aggregation &aggregation,
                                       const IdentifierFilter &filter)
{
    mAggregation = aggregation;
    mIdentifierFilter = filter;
    ui->filterEdit->setText("");
    ui->aggregationBox->setCurrentText(mAggregation.typeText());
    mAggregationMethod = ui->aggregationBox->currentIndex();
    ui->absoluteBox->setChecked(mAggregation.useAbsoluteValues());
    setAggregationMethodAvailability();
    setupAggregationView();
    if (aggregation.useAbsoluteValuesGlobal())
        ui->absoluteBox->setEnabled(false);
    else
        ui->absoluteBox->setEnabled(true);
}

void AggregationDialog::setDefaultAggregation(const Aggregation &aggregation)
{
    mDefaultAggregation = aggregation;
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
    setAggregation(mAggregation, mIdentifierFilter);
    close();
}

void AggregationDialog::on_resetButton_clicked()
{
    setAggregation(mDefaultAggregation, mIdentifierFilter);
    applyAggregation();
    emit aggregationUpdated();
}

void AggregationDialog::on_applyButton_clicked()
{
    if (ui->aggregationBox->currentIndex() == 0)
        return;
    applyAggregation();
    mAggregationMethod = ui->aggregationBox->currentIndex();
    emit aggregationUpdated();
}

void AggregationDialog::filterUpdate(const QString &text)
{
    if (!mAggregationModel) return;
    if (QRegExp("^\\d+$").exactMatch(text) || text.isEmpty()) {
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
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ? FilterTreeItem::VariableText :
                                                                       FilterTreeItem::EquationText,
                                       Qt::Unchecked, root);
    typeItem->setCheckable(false);
    auto identifierStates = mIdentifierFilter[orientation];
    Q_FOREACH(const auto& item, mAggregation.aggregationSymbols(orientation)) {
        if (item.checkStates().isEmpty())
            continue;
        if (identifierStates[item.symbolIndex()].Checked == Qt::Unchecked)
            continue;
        auto sItem = new FilterTreeItem(item.text(), Qt::Unchecked, typeItem);
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
            sItem->setSymbolIndex(item.symbolIndex());
            sItem->append(dItem);
        }
    }
    if (mAggregation.type() == Aggregation::MinMax)
        typeItem->setEnabled(false);
    typeItem->hasChildren() ? root->append(typeItem) : delete typeItem;
}

void AggregationDialog::setAggregationMethodAvailability()
{
    auto model = qobject_cast<QStandardItemModel*>(ui->aggregationBox->model());
    if (!model) return;
    for (int r=0; r<model->rowCount()-1; ++r) {
        auto item = model->item(r);
        if (!item) continue;
        if (mAggregation.viewType() == PredefinedViewEnum::MinMax) {
            auto flags = item->flags();
            flags.setFlag(Qt::ItemIsEnabled, false);
            item->setFlags(flags);
        } else {
            auto flags = item->flags();
            flags.setFlag(Qt::ItemIsEnabled, true);
            item->setFlags(flags);
        }
    }
    auto item = model->item(model->rowCount()-1);
    if (item && mAggregation.viewType() == PredefinedViewEnum::MinMax) {
        auto flags = item->flags();
        flags.setFlag(Qt::ItemIsEnabled, true);
        item->setFlags(flags);
    } else if (item) {
        auto flags = item->flags();
        flags.setFlag(Qt::ItemIsEnabled, false);
        item->setFlags(flags);
    }
}

void AggregationDialog::applyAggregation()
{
    mAggregation.setUseAbsoluteValues(ui->absoluteBox->isChecked());
    mAggregation.setType(ui->aggregationBox->currentText());
    QList<FilterTreeItem*> items {
        static_cast<FilterTreeModel*>(mAggregationModel->sourceModel())->filterItem()->childs()
    };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        if (item->text() == FilterTreeItem::EquationText) {
            mAggregation.setAggregationSymbols(Qt::Vertical, checkStates(item));
        } else if (item->text() == FilterTreeItem::VariableText) {
            mAggregation.setAggregationSymbols(Qt::Horizontal, checkStates(item));
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
        for (int d=0; d<item->childs().size(); ++d) {
            auto child = item->child(d);
            if (!child) continue;
            aggrItem.setCheckState(d+1, item->child(d)->checked());
        }
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
