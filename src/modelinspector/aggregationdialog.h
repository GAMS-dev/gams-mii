#ifndef AGGREGATIONDIALOG_H
#define AGGREGATIONDIALOG_H

#include <QDialog>

class QSortFilterProxyModel;
class QTreeView;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class AggregationDialog;
}

class FilterTreeItem;
class ModelInstance;
class TreeGenerator;

class AggregationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AggregationDialog(QWidget *parent = nullptr);
    ~AggregationDialog();

    bool isInitialized() const { // TODO remove?
        return mModelInstance != nullptr;
    }

    QString statusText() const;

    void setModelInstance(QSharedPointer<ModelInstance> modelInstance);

signals:
    void updated();

private slots:
    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void on_cancelButton_clicked();

    void on_applyButton_clicked();

    void leftFilterUpdate(const QString &text);

private:
    void setSelection(QSortFilterProxyModel *model, bool state);

    void setupLeftTreeView(FilterTreeItem *root);

    FilterTreeItem* setupAggregationItems(bool rightView);

private:
    Ui::AggregationDialog *ui;
    TreeGenerator *mTreeGenerator;
    QSharedPointer<ModelInstance> mModelInstance; // TODO remove?
    QSortFilterProxyModel *mFilterModel = nullptr;
    FilterTreeItem *mRootItem = nullptr;
};

}
}
}

#endif // AGGREGATIONDIALOG_H
