#include "aggregationdialog.h"
#include "ui_aggregationdialog.h"
#include "modelinstance.h"
#include "filtertreeitem.h"
#include "filtertreemodel.h"

#include <QStandardItem>
#include <QSortFilterProxyModel>

namespace gams {
namespace studio {
namespace modelinspector {

// TODO check box behaviour
// TODO Filter symbols and keep dimensions

class TreeGenerator {
public:
    void setModelInstance(QSharedPointer<ModelInstance> modelInstance)
    {
        mModelInstance = modelInstance;
    }

    void generate(FilterTreeItem *root)
    {
        if (!mModelInstance)
            return;

        auto eqnRoot = new FilterTreeItem(FilterTreeItem::EquationText, false, root);
        root->append(eqnRoot);
        appendSymbols(eqnRoot, mModelInstance->equations());

        auto varRoot = new FilterTreeItem(FilterTreeItem::VariableText, false, root);
        root->append(varRoot);
        appendSymbols(varRoot, mModelInstance->variables());
    }

private:
    void appendSymbols(FilterTreeItem *parent, const QVector<SymbolInfo> &symbols)
    {
        Q_FOREACH(auto sym, symbols) {
            if (!sym.Dimension)
                continue;
            auto item = new FilterTreeItem(sym.Name,
                                           Qt::Unchecked,
                                           -1, // TODO mapping
                                           parent);
            item->setCheckable(false);
            parent->append(item);
            appendDimensions(item, sym.Dimension);
        }
    }

    void appendDimensions(FilterTreeItem *parent, int dimensions)
    {
        for (int i=1; i<=dimensions; ++i) {
            auto dim = new FilterTreeItem(QString::number(i),
                                          Qt::Checked,
                                          -1, // TODO mapping
                                          parent);
            parent->append(dim);
        }
    }

private:
    QSharedPointer<ModelInstance> mModelInstance;
};

AggregationDialog::AggregationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AggregationDialog)
    , mTreeGenerator(new TreeGenerator)
{
    ui->setupUi(this);

    connect(ui->filterEdit, &QLineEdit::textChanged,
            this, &AggregationDialog::leftFilterUpdate);
    connect(this, &QDialog::rejected,
            this, &AggregationDialog::on_cancelButton_clicked);

    setupLeftTreeView(setupAggregationItems(false));
}

AggregationDialog::~AggregationDialog()
{
    delete ui;
    delete mTreeGenerator;
}

QString AggregationDialog::statusText() const
{// TODO show aggregation status None
    //if () {
    //    return "None";
    //}
    return ui->aggregationBox->currentText();
}

void AggregationDialog::setModelInstance(QSharedPointer<ModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
    mTreeGenerator->setModelInstance(modelInstance);

    setupLeftTreeView(setupAggregationItems(false));
}

void AggregationDialog::on_selectButton_clicked()
{
    setSelection(mFilterModel, true);
}

void AggregationDialog::on_deselectButton_clicked()
{
    setSelection(mFilterModel, false);
}

void AggregationDialog::on_cancelButton_clicked()
{
    close();
}

void AggregationDialog::on_applyButton_clicked()
{
    emit updated();
}

void AggregationDialog::leftFilterUpdate(const QString &text)
{
    if (!mFilterModel) return;
    if (QRegExp("^\\d+$").exactMatch(text) || text.isEmpty()) {
        mFilterModel->setFilterWildcard(text);
        ui->view->expandAll();
    }
}

void AggregationDialog::setSelection(QSortFilterProxyModel *model, bool state)
{
    QModelIndexList indexes;
    for(int row=0; row<model->rowCount(); ++row) {
        indexes.append(model->index(row, 0));
    }
    while (!indexes.isEmpty()) {
        auto index = indexes.takeFirst();
        if (!model->hasChildren(index))
            model->setData(index, state, Qt::CheckStateRole);
        if (model->hasChildren(index)) {
            for(int row=0; row<model->rowCount(index); ++row)
                indexes.append(model->index(row, 0, index));
        }
    }
}

void AggregationDialog::setupLeftTreeView(FilterTreeItem *root)
{
    auto oldModel = ui->view->selectionModel();
    auto treeModel = new FilterTreeModel(root, ui->view);
    mFilterModel = new QSortFilterProxyModel(ui->view);
    mFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mFilterModel->setRecursiveFilteringEnabled(true);
    mFilterModel->setSourceModel(treeModel);
    ui->view->setModel(mFilterModel);
    ui->view->expandAll();
    if (oldModel)
        oldModel->deleteLater();
}

FilterTreeItem* AggregationDialog::setupAggregationItems(bool rightView)
{
    mRootItem = new FilterTreeItem;
    mTreeGenerator->generate(mRootItem);
    return mRootItem;
}

}
}
}
