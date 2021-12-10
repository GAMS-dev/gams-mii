#include "aggregationdialog.h"
#include "ui_aggregationdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>

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
    setupAggregationView();
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
    ui->filterEdit->setText("");
    close();
}

void AggregationDialog::on_resetButton_clicked()
{
    setAggregation(mDefaultAggregation, mIdentifierFilter);
    ui->filterEdit->setText("");
    mAggregationMethod = 0;
    ui->aggregationBox->setCurrentIndex(mAggregationMethod);
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
    auto rootItem = new FilterTreeItem(QString(), false);
    setupTreeItems(Qt::Vertical, rootItem);
    setupTreeItems(Qt::Horizontal, rootItem);

    auto oldVarModel = ui->view->selectionModel();
    auto treeModel = new FilterTreeModel(rootItem, ui->view);
    mAggregationModel = new QSortFilterProxyModel(ui->view);
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
                                       Qt::Unchecked, -1, root);
    typeItem->setCheckable(false);
    auto identifierStates = mIdentifierFilter[orientation];
    Q_FOREACH(const auto& item, mAggregation.aggregationSymbols(orientation)) {
        if (item.checkState().isEmpty())
            continue;
        if (identifierStates[item.symbolIndex()].Checked == Qt::Unchecked)
            continue;
        auto sItem = new FilterTreeItem(item.text(), Qt::Unchecked,
                                        item.symbolIndex(), typeItem);
        sItem->setCheckable(false);
        typeItem->append(sItem);
        for (auto iter=item.checkState().keyValueBegin();
             iter!=item.checkState().constKeyValueEnd(); ++iter)
        {
            auto dItem = new FilterTreeItem(QString::number(iter->first),
                                            iter->second,
                                            item.symbolIndex(),
                                            sItem);
            sItem->append(dItem);
        }
    }
    typeItem->hasChildren() ? root->append(typeItem) : delete typeItem;
}

void AggregationDialog::applyAggregation()
{
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
        aggrItem.setSymbolIndex(item->index());
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
