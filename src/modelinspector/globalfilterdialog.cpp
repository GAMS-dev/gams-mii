#include "globalfilterdialog.h"
#include "ui_globalfilterdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

GlobalFilterDialog::GlobalFilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GlobalFilterDialog)
{
    ui->setupUi(this);
    ui->labelView->sortByColumn(0, Qt::AscendingOrder);
    ui->minEdit->setValidator(new QDoubleValidator(ui->minEdit));
    ui->maxEdit->setValidator(new QDoubleValidator(ui->maxEdit));

    connect(this, &QDialog::rejected,
            this, &GlobalFilterDialog::on_cancelButton_clicked);
}

GlobalFilterDialog::~GlobalFilterDialog()
{
    delete ui;
}

IdentifierFilter GlobalFilterDialog::idendifierFilter() const
{
    return mIdentifierFilter;
}

void GlobalFilterDialog::setIdentifierFilter(const IdentifierFilter &filter)
{
    mIdentifierFilter = filter;
    setupEquationFilter(mIdentifierFilter[Qt::Vertical]);
    setupVariableFilter(mIdentifierFilter[Qt::Horizontal]);
}

void GlobalFilterDialog::setDefaultIdentifierFilter(const IdentifierFilter &filter)
{
    mDefaultIdentifierFilter = filter;
}

ValueFilter GlobalFilterDialog::valueFilter() const
{
    return mValueFilter;
}

void GlobalFilterDialog::setValueFilter(const ValueFilter &filter)
{
    mValueFilter = filter;
    ui->minEdit->setText(QString::number(mValueFilter.MinValue));
    ui->maxEdit->setText(QString::number(mValueFilter.MaxValue));
    ui->excludeBox->setChecked(mValueFilter.Exclude);
    ui->epsBox->setChecked(mValueFilter.ShowEps);
    ui->nInfBox->setChecked(mValueFilter.ShowNInf);
    ui->pInfBox->setChecked(mValueFilter.ShowPInf);
}

void GlobalFilterDialog::setDefaultValueFilter(const ValueFilter &filter)
{
    mDefaultValueFilter = filter;
}

LabelFilter GlobalFilterDialog::labelFilter() const
{
    return mLabelFilter;
}

void GlobalFilterDialog::setLabelFilter(const LabelFilter &filter)
{
    mLabelFilter = filter;
    setupLabelFilter();
}

void GlobalFilterDialog::setDefaultLabelFilter(const LabelFilter &filter)
{
    mDefaultLabelFilter = filter;
}

void GlobalFilterDialog::on_applyButton_clicked()
{
    mIdentifierFilter[Qt::Horizontal] = applyHeaderFilter(mVarFilterModel);
    mIdentifierFilter[Qt::Vertical] = applyHeaderFilter(mEqnFilterModel);
    applyValueFilter();
    mLabelFilter[Qt::Horizontal] = applyLabelFilter(Qt::Horizontal, mLabelFilterModel);
    mLabelFilter[Qt::Vertical] = applyLabelFilter(Qt::Vertical, mLabelFilterModel);
    emit filterUpdated();
}

void GlobalFilterDialog::on_resetButton_clicked()
{
    setIdentifierFilter(mDefaultIdentifierFilter);
    setValueFilter(mDefaultValueFilter);
    setLabelFilter(mDefaultLabelFilter);

    ui->equationEdit->setText("");
    ui->variableEdit->setText("");
    ui->labelEdit->setText("");
    ui->labelFilterBox->setCurrentIndex(0);

    on_applyButton_clicked();
}

void GlobalFilterDialog::on_cancelButton_clicked()
{
    setIdentifierFilter(mIdentifierFilter);
    setValueFilter(mValueFilter);
    setLabelFilter(mLabelFilter);

    ui->equationEdit->setText("");
    ui->variableEdit->setText("");
    ui->labelEdit->setText("");
    ui->labelFilterBox->setCurrentIndex(0);

    close();
}

void GlobalFilterDialog::on_selectEqnButton_clicked()
{
    applyCheckState(ui->equationView, mEqnFilterModel, Qt::Checked);
    ui->equationView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectEqnButton_clicked()
{
    applyCheckState(ui->equationView, mEqnFilterModel, Qt::Unchecked);
    ui->equationView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_selectVarButton_clicked()
{
    applyCheckState(ui->variableView, mVarFilterModel, Qt::Checked);
    ui->variableView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectVarButton_clicked()
{
    applyCheckState(ui->variableView, mVarFilterModel, Qt::Unchecked);
    ui->variableView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_selectLabelButton_clicked()
{
    applyCheckState(ui->labelView, mLabelFilterModel, Qt::Checked);
    ui->labelView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectLabelButton_clicked()
{
    applyCheckState(ui->labelView, mLabelFilterModel, Qt::Unchecked);
    ui->labelView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_labelFilterBox_currentIndexChanged(int index)
{
    switch (index) {
    case 1: // Equations
        ui->labelView->setRowHidden(1, QModelIndex(), true);
        ui->labelView->setRowHidden(0, QModelIndex(), false);
        break;
    case 2: // Variables
        ui->labelView->setRowHidden(1, QModelIndex(), false);
        ui->labelView->setRowHidden(0, QModelIndex(), true);
        break;
    default: // All
        ui->labelView->reset();
        ui->labelView->expandAll();
        break;
    }
}

void GlobalFilterDialog::setupEquationFilter(const IdentifierStates &filter)
{
    auto filterItem = setupSymTreeItems(Qt::Vertical, filter);
    auto oldEqnModel = ui->equationView->selectionModel();
    auto eqnTreeModel = new FilterTreeModel(filterItem, ui->equationView);
    mEqnFilterModel = new QSortFilterProxyModel(ui->equationView);
    mEqnFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mEqnFilterModel->setRecursiveFilteringEnabled(true);
    mEqnFilterModel->setSourceModel(eqnTreeModel);
    ui->equationView->setModel(mEqnFilterModel);
    ui->equationView->expandAll();
    if (oldEqnModel)
        oldEqnModel->deleteLater();
    connect(ui->equationEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                            if (!mEqnFilterModel) return;
                            mEqnFilterModel->setFilterWildcard(text);
                            ui->equationView->expandAll();
                        });
}

void GlobalFilterDialog::setupVariableFilter(const IdentifierStates &filter)
{
    auto filterItem = setupSymTreeItems(Qt::Horizontal, filter);
    auto oldVarModel = ui->variableView->selectionModel();
    auto varTreeModel = new FilterTreeModel(filterItem, ui->variableView);
    mVarFilterModel = new QSortFilterProxyModel(ui->variableEdit);
    mVarFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mVarFilterModel->setRecursiveFilteringEnabled(true);
    mVarFilterModel->setSourceModel(varTreeModel);
    ui->variableView->setModel(mVarFilterModel);
    ui->variableView->expandAll();
    if (oldVarModel)
        oldVarModel->deleteLater();
    connect(ui->variableEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) {
                                                if (!mVarFilterModel) return;
                                                mVarFilterModel->setFilterWildcard(text);
                                                ui->variableView->expandAll();
                                              });
}

void GlobalFilterDialog::setupLabelFilter()
{
    auto labelFilterItem = new FilterTreeItem("", Qt::Unchecked);
    labelFilterItem->setCheckable(false);
    setupLabelTreeItems(Qt::Horizontal, labelFilterItem);
    setupLabelTreeItems(Qt::Vertical, labelFilterItem);

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

FilterTreeItem* GlobalFilterDialog::setupSymTreeItems(Qt::Orientation orientation,
                                                      const IdentifierStates &filter)
{
    auto root = new FilterTreeItem(QString(), Qt::Unchecked);
    root->setCheckable(false);
    auto attributes = new FilterTreeItem("Attributes", Qt::Unchecked, -1, root);
    attributes->setCheckable(false);
    root->append(attributes);
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ? FilterTreeItem::VariableText :
                                                                       FilterTreeItem::EquationText,
                                       Qt::Unchecked, -1, root);
    typeItem->setCheckable(false);
    root->append(typeItem);
    Q_FOREACH(auto item, filter) {
        if (item.SymbolIndex < PredefinedHeaderLength) {
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            item.SymbolIndex,
                                            attributes);
            fItem->setEnabled(item.Enabled);
            attributes->append(fItem);
        } else {
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            item.SymbolIndex,
                                            typeItem);
            typeItem->append(fItem);
        }
    }
    return root;
}

void GlobalFilterDialog::setupLabelTreeItems(Qt::Orientation orientation, FilterTreeItem *root)
{
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ?
                                           FilterTreeItem::VariableText :
                                           FilterTreeItem::EquationText,
                                       Qt::Unchecked, -1, root);
    typeItem->setCheckable(false);
    root->append(typeItem);

    auto labelFilter = mLabelFilter[orientation];
    for (auto iter=labelFilter.constKeyValueBegin(); iter!=labelFilter.constKeyValueEnd(); ++iter) {
        auto treeItem = new FilterTreeItem(iter->first,
                                           iter->second ? Qt::Checked : Qt::Unchecked,
                                           -1, typeItem);
        typeItem->append(treeItem);
    }
}

void GlobalFilterDialog::applyCheckState(QTreeView *view,
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

IdentifierStates GlobalFilterDialog::applyHeaderFilter(QSortFilterProxyModel *model)
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
        identifierState.SymbolIndex = item->index();
        identifierState.Text =  item->text();
        identifierState.Checked = item->checked();
        filter[item->index()] = identifierState;
    }
    return filter;
}

void GlobalFilterDialog::applyValueFilter()
{
    mValueFilter.MinValue = ui->minEdit->text().toDouble();
    mValueFilter.MaxValue = ui->maxEdit->text().toDouble();
    mValueFilter.Exclude = ui->excludeBox->isChecked();
    mValueFilter.ShowPInf = ui->pInfBox->isChecked();
    mValueFilter.ShowNInf = ui->nInfBox->isChecked();
    mValueFilter.ShowEps = ui->epsBox->isChecked();
}

LabelStates GlobalFilterDialog::applyLabelFilter(Qt::Orientation orientation,
                                                 QSortFilterProxyModel *model)
{
    FilterTreeItem* root = nullptr;
    auto childs = static_cast<FilterTreeModel*>(model->sourceModel())->filterItem()->childs();
    Q_FOREACH(auto child, childs) {
        if (orientation == Qt::Horizontal && child->text() == FilterTreeItem::VariableText) {
            root = child;
            break;
        } else if (orientation == Qt::Vertical && child->text() == FilterTreeItem::EquationText) {
            root = child;
            break;
        }
    }

    LabelStates filter;
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

}
}
}
