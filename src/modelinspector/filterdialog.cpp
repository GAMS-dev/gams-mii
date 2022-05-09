#include "filterdialog.h"
#include "ui_filterdialog.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QSortFilterProxyModel>

#include <QDebug>

namespace gams {
namespace studio {
namespace modelinspector {

// TODO consolidate filters, aggregation, row/column data source... struct/class?

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
    connect(ui->minEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) { updateRangeEdit(ui->minEdit, text); });
    connect(ui->maxEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) { updateRangeEdit(ui->maxEdit, text); });
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
    setupEquationFilter(mIdentifierFilter[equationOrientation()]);
    setupVariableFilter(mIdentifierFilter[variableOrientation()]);
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
    ui->maxEdit->setText(QString::number(mValueFilter.MaxValue));
    ui->excludeBox->setChecked(mValueFilter.ExcludeRange);
    ui->absoluteBox->setChecked(mValueFilter.UseAbsoluteValues);
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

DataSource FilterDialog::horizontalDataSource() const
{
    return mHorizontalDataSource;
}

void FilterDialog::setHorizontalDataSource(DataSource dataSource)
{
    mHorizontalDataSource = dataSource;
}

DataSource FilterDialog::verticalDataSource() const
{
    return mVerticalDataSource;
}

void FilterDialog::setVerticalDataSource(DataSource dataSource)
{
    mVerticalDataSource = dataSource;
}

PredefinedViewEnum FilterDialog::viewType() const
{
    return mViewType;
}

void FilterDialog::setViewType(PredefinedViewEnum viewType)
{
    mViewType = viewType;
}

void FilterDialog::on_applyButton_clicked()
{
    mIdentifierFilter[variableOrientation()] = applyHeaderFilter(mVarFilterModel, DataSource::VariableData);
    mIdentifierFilter[equationOrientation()] = applyHeaderFilter(mEqnFilterModel, DataSource::EquationData);
    applyValueFilter();
    mLabelFilter.LabelCheckStates[variableOrientation()] = applyLabelFilter(variableOrientation(), mLabelFilterModel);
    mLabelFilter.LabelCheckStates[equationOrientation()] = applyLabelFilter(equationOrientation(), mLabelFilterModel);
    mLabelFilter.ColumnDataSource = mHorizontalDataSource;
    mLabelFilter.RowDataSource = mVerticalDataSource;
    mLabelFilter.Any = ui->labelBox->currentIndex();
    emit filterUpdated();
}

void FilterDialog::on_resetButton_clicked()
{
    setIdentifierFilter(mDefaultIdentifierFilter);
    setValueFilter(mDefaultValueFilter);
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

void FilterDialog::setupEquationFilter(const IdentifierStates &filter)
{
    bool showAttributes = !(mViewType == PredefinedViewEnum::EqnAttributes ||
                           mViewType == PredefinedViewEnum::Jaccobian);
    bool showSymbols = mViewType != PredefinedViewEnum::VarAttributes;
    auto filterItem = setupSymTreeItems(FilterTreeItem::EquationText, filter,
                                        showAttributes, showSymbols);
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

void FilterDialog::setupVariableFilter(const IdentifierStates &filter)
{
    bool showAttributes = !(mViewType == PredefinedViewEnum::VarAttributes ||
                            mViewType == PredefinedViewEnum::Jaccobian);
    bool showSymbols = mViewType != PredefinedViewEnum::EqnAttributes;
    auto filterItem = setupSymTreeItems(FilterTreeItem::VariableText, filter,
                                        showAttributes, showSymbols);
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
                                                bool showAttributes,
                                                bool showSymbols)
{
    auto root = new FilterTreeItem(QString(), Qt::Unchecked);
    root->setCheckable(false);
    auto attributes = new FilterTreeItem(FilterTreeItem::AttributesText, Qt::Unchecked, root);
    attributes->setCheckable(false);
    auto symbols = new FilterTreeItem(text, Qt::Unchecked, root);
    symbols->setCheckable(false);
    Q_FOREACH(auto item, filter) {
        if (item.SymbolIndex < PredefinedHeaderLength) {
            if (!showAttributes) continue;
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            attributes);
            fItem->setSymbolIndex(item.SymbolIndex);
            fItem->setEnabled(item.Enabled);
            attributes->append(fItem);
        } else {
            if (!showSymbols) continue;
            auto fItem = new FilterTreeItem(item.Text,
                                            item.Checked,
                                            symbols);
            fItem->setSymbolIndex(item.SymbolIndex);
            symbols->append(fItem);
        }
    }
    showAttributes ? root->append(attributes) : delete attributes;
    showSymbols ? root->append(symbols) : delete symbols;
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

IdentifierStates FilterDialog::applyHeaderFilter(QSortFilterProxyModel *model,
                                                 DataSource dataSource)
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
        identifierState.SymbolType = dataSource;
        identifierState.Text =  item->text();
        identifierState.Checked = item->checked();
        filter[item->symbolIndex()] = identifierState;
    }
    return filter;
}

void FilterDialog::applyValueFilter()
{
    mValueFilter.MinValue = ui->minEdit->text().toDouble();
    mValueFilter.MaxValue = ui->maxEdit->text().toDouble();
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

void FilterDialog::disableAttributes(QSortFilterProxyModel *model)
{
    auto* item = static_cast<FilterTreeModel*>(model->sourceModel())->filterItem();
    auto attrItem = item->findChild(FilterTreeItem::AttributesText);
    if (!attrItem) return;
    Q_FOREACH(auto child, attrItem->childs()) {
        child->setChecked(Qt::Unchecked);
    }
}

}
}
}
