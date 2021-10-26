#include "globalfilterdialog.h"
#include "ui_globalfilterdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"
#include "modelinstance.h"

#include <QMap>
#include <QStandardItem>
#include <QSortFilterProxyModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

GlobalFilterDialog::GlobalFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalFilterDialog)
{
    ui->setupUi(this);
    ui->uelView->sortByColumn(0, Qt::AscendingOrder);
    ui->minEdit->setValidator(new QDoubleValidator(ui->minEdit));
    ui->maxEdit->setValidator(new QDoubleValidator(ui->maxEdit));

    connect(this, &QDialog::rejected,
            this, &GlobalFilterDialog::on_cancelButton_clicked);
    connect(ui->minEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                    mSettings.MinValue = text.toDouble(); });
    connect(ui->maxEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                    mSettings.MaxValue = text.toDouble(); });
    connect(ui->excludeBox, &QCheckBox::stateChanged,
            this, [this](int state){ mSettings.Exclude = state; });
    connect(ui->pInfBox, &QCheckBox::stateChanged,
            this, [this](int state){ mSettings.ShowPInf = state; });
    connect(ui->nInfBox, &QCheckBox::stateChanged,
            this, [this](int state){ mSettings.ShowNInf = state; });
    connect(ui->epsBox, &QCheckBox::stateChanged,
            this, [this](int state){ mSettings.ShowEps = state; });
}

GlobalFilterDialog::~GlobalFilterDialog()
{
    delete ui;
}

void GlobalFilterDialog::setModelInstance(QSharedPointer<ModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mSettings = mModelInstance->valueFilterSettings();
    ui->minEdit->setText(QString::number(mSettings.MinValue));
    ui->maxEdit->setText(QString::number(mSettings.MaxValue));
    ui->excludeBox->setChecked(mSettings.Exclude);
    ui->epsBox->setChecked(mSettings.ShowEps);
    ui->nInfBox->setChecked(mSettings.ShowNInf);
    ui->pInfBox->setChecked(mSettings.ShowPInf);
    setupEquationFilter();
    setupVariableFilter();
    setupUelsFilter();
}

void GlobalFilterDialog::reloadUpdatedFilterData()
{
    mSettings = mModelInstance->valueFilterSettings();
    setModelInstance(mModelInstance);
    applyEqnFilter(ui->equationEdit->text());
    applyVarFilter(ui->variableEdit->text());
    applyUelFilter(ui->uelEdit->text());
}

void GlobalFilterDialog::on_applyButton_clicked()
{
    updateEqnVarData(Qt::Horizontal,
                     static_cast<FilterTreeModel*>(mVarFilterModel->sourceModel())->filterItem());
    updateEqnVarData(Qt::Vertical,
                     static_cast<FilterTreeModel*>(mEqnFilterModel->sourceModel())->filterItem());

    auto hUelStates = uelSatesFromFilterView(Qt::Horizontal);
    mModelInstance->setUelStates(Qt::Horizontal, hUelStates);

    auto vUelStates = uelSatesFromFilterView(Qt::Vertical);
    mModelInstance->setUelStates(Qt::Vertical, vUelStates);

    mModelInstance->setValueFilterSettings(mSettings);
    emit updated();
}

void GlobalFilterDialog::on_resetButton_clicked()
{
    mSettings = mModelInstance->valueFilterSettings();
    setModelInstance(mModelInstance);
    ui->equationEdit->setText("");
    ui->variableEdit->setText("");
    ui->uelEdit->setText("");
    ui->uelFilterBox->setCurrentIndex(0);
}

void GlobalFilterDialog::on_cancelButton_clicked()
{
    on_resetButton_clicked();
    close();
}

void GlobalFilterDialog::on_selectEqnButton_clicked()
{
    setCheckState(mEqnFilterModel, Qt::Checked);
    ui->equationView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectEqnButton_clicked()
{
    setCheckState(mEqnFilterModel, Qt::Unchecked);
    ui->equationView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_selectVarButton_clicked()
{
    setCheckState(mVarFilterModel, Qt::Checked);
    ui->variableView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectVarButton_clicked()
{
    setCheckState(mVarFilterModel, Qt::Unchecked);
    ui->variableView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_selectUelButton_clicked()
{
    setCheckState(mUelFilterModel, Qt::Checked);
    ui->uelView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectUelButton_clicked()
{
    setCheckState(mUelFilterModel, Qt::Unchecked);
    ui->uelView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_uelFilterBox_currentIndexChanged(int index)
{
    switch (index) {
    case 1: // Equations
        ui->uelView->setRowHidden(1, QModelIndex(), true);
        ui->uelView->setRowHidden(0, QModelIndex(), false);
        break;
    case 2: // Variables
        ui->uelView->setRowHidden(1, QModelIndex(), false);
        ui->uelView->setRowHidden(0, QModelIndex(), true);
        break;
    default: // All
        ui->uelView->reset();
        ui->uelView->expandAll();
        break;
    }
}

void GlobalFilterDialog::applyEqnFilter(const QString &text)
{
    if (!mEqnFilterModel) return;
    mEqnFilterModel->setFilterWildcard(text);
    ui->equationView->expandAll();
}

void GlobalFilterDialog::applyVarFilter(const QString &text)
{
    if (!mVarFilterModel) return;
    mVarFilterModel->setFilterWildcard(text);
    ui->variableView->expandAll();
}

void GlobalFilterDialog::applyUelFilter(const QString &text)
{
    mUelFilterModel->setFilterWildcard(text);
    ui->uelView->expandAll();
}

void GlobalFilterDialog::setupEquationFilter()
{
    auto filterItem = setupEqnVarFilterItems(Qt::Vertical,
                                             mModelInstance->verticalSymbols());
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
            this, &GlobalFilterDialog::applyEqnFilter);
}

void GlobalFilterDialog::setupVariableFilter()
{
    auto filterItem = setupEqnVarFilterItems(Qt::Horizontal,
                                             mModelInstance->horizontalSymbols());
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
            this, &GlobalFilterDialog::applyVarFilter);
}

FilterTreeItem* GlobalFilterDialog::setupEqnVarFilterItems(Qt::Orientation orientation,
                                                           const QList<QStandardItem*> items)
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
    Q_FOREACH(auto item, items) {
        auto itemIndex = mModelInstance->itemToIndex(orientation, item);
        if (itemIndex < mModelInstance->predefinedHeaderLength()) {
            auto fitem = new FilterTreeItem(item->data(Qt::DisplayRole).toString(),
                                            item->checkState(),
                                            itemIndex,
                                            attributes);
            attributes->append(fitem);
        } else {
            auto fitem = new FilterTreeItem(item->data(Qt::DisplayRole).toString(),
                                            item->checkState(),
                                            itemIndex,
                                            typeItem);
            typeItem->append(fitem);
        }
    }
    return root;
}

void GlobalFilterDialog::setupUelsFilter()
{
    auto uelFilterItem = new FilterTreeItem("", Qt::Unchecked);
    uelFilterItem->setCheckable(false);
    setupUelFilterItems(Qt::Horizontal, uelFilterItem);
    setupUelFilterItems(Qt::Vertical, uelFilterItem);

    auto oldUelModel = ui->uelView->selectionModel();
    mUelTreeModel = new FilterTreeModel(uelFilterItem, ui->uelView);
    mUelFilterModel = new QSortFilterProxyModel(ui->uelView);
    mUelFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mUelFilterModel->setRecursiveFilteringEnabled(true);
    mUelFilterModel->setSourceModel(mUelTreeModel);
    ui->uelView->setModel(mUelFilterModel);
    ui->uelView->expandAll();
    if (oldUelModel)
        oldUelModel->deleteLater();
    connect(ui->uelEdit, &QLineEdit::textChanged,
            this, &GlobalFilterDialog::applyUelFilter);
}

void GlobalFilterDialog::setupUelFilterItems(Qt::Orientation orientation,
                                             FilterTreeItem *root)
{
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ?
                                           FilterTreeItem::VariableText :
                                           FilterTreeItem::EquationText,
                                       Qt::Unchecked, -1, root);
    typeItem->setCheckable(false);
    root->append(typeItem);
    auto uelStates = mModelInstance->uelStates(orientation);
    for (auto iter=uelStates.constKeyValueBegin(); iter!=uelStates.constKeyValueEnd(); ++iter) {
        auto treeItem = new FilterTreeItem(iter->first,
                                           iter->second ? Qt::Checked : Qt::Unchecked,
                                           -1, typeItem);
        typeItem->append(treeItem);
    }
}

void GlobalFilterDialog::updateEqnVarData(Qt::Orientation orientation,
                                          FilterTreeItem *filterTree)
{
    QList<FilterTreeItem*> items { filterTree };
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        items.append(item->childs());
        if (item->isCheckable()) {
            QStandardItem *stdItem;
            if (orientation == Qt::Horizontal) {
                stdItem = mModelInstance->horizontalItem(item->logicalIndex());
                mModelInstance->setHeaderRootItemEnabled(stdItem, item->checked());
            } else {
                stdItem = mModelInstance->verticalItem(item->logicalIndex());
                mModelInstance->setHeaderRootItemEnabled(stdItem, item->checked());
            }
        }
    }
}

void GlobalFilterDialog::setCheckState(QSortFilterProxyModel *filterModel,
                                       Qt::CheckState state)
{
    QModelIndexList indexes;
    for(int row=0; row<filterModel->rowCount(); ++row) {
        indexes.append(filterModel->index(row, 0));
    }
    while (!indexes.isEmpty()) {
        auto index = indexes.takeFirst();
        filterModel->setData(index, state, Qt::CheckStateRole);
        if (filterModel->hasChildren(index)) {
            for(int row=0; row<filterModel->rowCount(index); ++row)
                indexes.append(filterModel->index(row, 0, index));
        }
    }
}

QMap<QString, bool> GlobalFilterDialog::uelSatesFromFilterView(Qt::Orientation orientation)
{
    QMap<QString, bool> uels;
    if (!mUelTreeModel->filterItem())
        return uels;
    FilterTreeItem *root = nullptr;
    Q_FOREACH(auto item, mUelTreeModel->filterItem()->childs()) {
        if (orientation == Qt::Horizontal && item->text() == FilterTreeItem::VariableText) {
            root = item;
            break;
        } else if (orientation == Qt::Vertical && item->text() == FilterTreeItem::EquationText) {
            root = item;
            break;
        }
    }
    if (!root)
        return uels;
    Q_FOREACH(auto item, root->childs()) {
        uels[item->text()] = item->checked();
    }
    return uels;
}

}
}
}
