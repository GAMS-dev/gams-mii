#include "globalfilterdialog.h"
#include "ui_globalfilterdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>

namespace gams {
namespace studio {
namespace modelinspector {

GlobalFilterDialog::GlobalFilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GlobalFilterDialog)
{
    ui->setupUi(this);
    ui->uelView->sortByColumn(0, Qt::AscendingOrder);
    ui->minEdit->setValidator(new QDoubleValidator(ui->minEdit));
    ui->maxEdit->setValidator(new QDoubleValidator(ui->maxEdit));

    connect(this, &QDialog::rejected,
            this, &GlobalFilterDialog::on_cancelButton_clicked);
}

GlobalFilterDialog::~GlobalFilterDialog()
{
    delete ui;
}

QMap<Qt::Orientation, SymbolFilter> GlobalFilterDialog::symbolFilter() const
{
    return mSymbolFilter;
}

void GlobalFilterDialog::setSymbolFilter(const SymbolFilterMap &filter)
{
    mSymbolFilter = filter;
    setupEquationFilter(mSymbolFilter[Qt::Vertical]);
    setupVariableFilter(mSymbolFilter[Qt::Horizontal]);
}

void GlobalFilterDialog::setDefaultSymbolFilter(const SymbolFilterMap &filter)
{
    mDefaultSymbolFilter = filter;
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

UelFilterMap GlobalFilterDialog::uelFilter() const
{
    return mUelFilter;
}

void GlobalFilterDialog::setUelFilter(const UelFilterMap &filter)
{
    mUelFilter = filter;
    setupUelFilter();
}

void GlobalFilterDialog::setDefaultUelFilter(const UelFilterMap &filter)
{
    mDefaultUelFilter = filter;
}

void GlobalFilterDialog::on_applyButton_clicked()
{
    mSymbolFilter[Qt::Horizontal] = applyHeaderFilter(mVarFilterModel);
    mSymbolFilter[Qt::Vertical] = applyHeaderFilter(mEqnFilterModel);
    applyValueFilter();
    mUelFilter[Qt::Horizontal] = applyUelFilter(Qt::Horizontal, mUelFilterModel);
    mUelFilter[Qt::Vertical] = applyUelFilter(Qt::Vertical, mUelFilterModel);
    emit filterUpdated();
}

void GlobalFilterDialog::on_resetButton_clicked()
{
    setSymbolFilter(mDefaultSymbolFilter);
    setValueFilter(mDefaultValueFilter);
    setUelFilter(mDefaultUelFilter);

    ui->equationEdit->setText("");
    ui->variableEdit->setText("");
    ui->uelEdit->setText("");
    ui->uelFilterBox->setCurrentIndex(0);

    on_applyButton_clicked();
}

void GlobalFilterDialog::on_cancelButton_clicked()
{
    setSymbolFilter(mSymbolFilter);
    setValueFilter(mValueFilter);
    setUelFilter(mUelFilter);

    ui->equationEdit->setText("");
    ui->variableEdit->setText("");
    ui->uelEdit->setText("");
    ui->uelFilterBox->setCurrentIndex(0);

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

void GlobalFilterDialog::on_selectUelButton_clicked()
{
    applyCheckState(ui->uelView, mUelFilterModel, Qt::Checked);
    ui->uelView->dataChanged(QModelIndex(), QModelIndex());
}

void GlobalFilterDialog::on_deselectUelButton_clicked()
{
    applyCheckState(ui->uelView, mUelFilterModel, Qt::Unchecked);
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

void GlobalFilterDialog::setupEquationFilter(const SymbolFilter &filter)
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

void GlobalFilterDialog::setupVariableFilter(const SymbolFilter &filter)
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

void GlobalFilterDialog::setupUelFilter()
{
    auto uelFilterItem = new FilterTreeItem("", Qt::Unchecked);
    uelFilterItem->setCheckable(false);
    setupUelTreeItems(Qt::Horizontal, uelFilterItem);
    setupUelTreeItems(Qt::Vertical, uelFilterItem);

    auto oldUelModel = ui->uelView->selectionModel();
    auto uelTreeModel = new FilterTreeModel(uelFilterItem, ui->uelView);
    mUelFilterModel = new QSortFilterProxyModel(ui->uelView);
    mUelFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mUelFilterModel->setRecursiveFilteringEnabled(true);
    mUelFilterModel->setSourceModel(uelTreeModel);
    ui->uelView->setModel(mUelFilterModel);
    ui->uelView->expandAll();
    if (oldUelModel)
        oldUelModel->deleteLater();
    connect(ui->uelEdit, &QLineEdit::textChanged,
            this, [this](const QString &text){
                                                mUelFilterModel->setFilterWildcard(text);
                                                ui->uelView->expandAll();
                                              });
}

FilterTreeItem* GlobalFilterDialog::setupSymTreeItems(Qt::Orientation orientation,
                                                   const SymbolFilter &filter)
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
        if (item.SectionIndex < PredefinedHeaderLength) {
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            item.SectionIndex,
                                            attributes);
            fItem->setEnabled(item.Enabled);
            attributes->append(fItem);
        } else {
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            item.SectionIndex,
                                            typeItem);
            typeItem->append(fItem);
        }
    }
    return root;
}

void GlobalFilterDialog::setupUelTreeItems(Qt::Orientation orientation, FilterTreeItem *root)
{
    auto typeItem = new FilterTreeItem(orientation == Qt::Horizontal ?
                                           FilterTreeItem::VariableText :
                                           FilterTreeItem::EquationText,
                                       Qt::Unchecked, -1, root);
    typeItem->setCheckable(false);
    root->append(typeItem);

    auto uelFilter = mUelFilter[orientation];
    for (auto iter=uelFilter.constKeyValueBegin(); iter!=uelFilter.constKeyValueEnd(); ++iter) {
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

SymbolFilter GlobalFilterDialog::applyHeaderFilter(QSortFilterProxyModel *model)
{
    QList<FilterTreeItem*> items {
        static_cast<FilterTreeModel*>(model->sourceModel())->filterItem()
    };
    SymbolFilter filter;
    while (!items.isEmpty()) {
        auto item = items.takeFirst();
        items.append(item->childs());
        if (!item->isCheckable())
            continue;
        filter.push_back(SymbolFilterItem {
                             item->isEnabled(), item->index(), item->text(), item->checked()
                         });
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

UelFilter GlobalFilterDialog::applyUelFilter(Qt::Orientation orientation,
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

    UelFilter filter;
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
