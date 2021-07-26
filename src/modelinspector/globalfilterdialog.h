#ifndef GLOBALFILTERDIALOG_H
#define GLOBALFILTERDIALOG_H

#include <QDialog>

#include "valuefiltersettings.h"

class QSortFilterProxyModel;
class QStandardItem;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class GlobalFilterDialog;
}

class FilterTreeItem;
class FilterTreeModel;
class ModelInstance;

class GlobalFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalFilterDialog(QWidget *parent = nullptr);
    ~GlobalFilterDialog();

    bool isInitialized() const {
        return mModelInstance != nullptr;
    }

    void setModelInstance(QSharedPointer<ModelInstance> modelInstance);

    void reloadUpdatedFilterData();

signals:
    void filterUpdate();

private slots:
    void on_applyButton_clicked();

    void on_resetButton_clicked();

    void on_cancelButton_clicked();

    void on_selectEqnButton_clicked();

    void on_deselectEqnButton_clicked();

    void on_selectVarButton_clicked();

    void on_deselectVarButton_clicked();

    void on_selectUelButton_clicked();

    void on_deselectUelButton_clicked();

    void on_uelFilterBox_currentIndexChanged(int index);

    void applyEqnFilter(const QString &text);
    void applyVarFilter(const QString &text);
    void applyUelFilter(const QString &text);

private:
    void setupEquationFilter();
    void setupVariableFilter();
    FilterTreeItem* setupEqnVarFilterItems(Qt::Orientation orientation,
                                           const QList<QStandardItem*> items);

    void setupUelsFilter();
    void setupUelFilterItems(Qt::Orientation orientation, FilterTreeItem *root);

    // TODO move to ModelInstance
    void updateEqnVarData(Qt::Orientation orientation, FilterTreeItem *filterTree);

    void setCheckState(QSortFilterProxyModel *filterModel, Qt::CheckState state);

    QMap<QString, bool> uelSatesFromFilterView(Qt::Orientation orientation);

private:
    Ui::GlobalFilterDialog *ui;
    ValueFilterSettings mSettings;
    QSharedPointer<ModelInstance> mModelInstance;
    QSortFilterProxyModel *mEqnFilterModel;
    QSortFilterProxyModel *mVarFilterModel;
    FilterTreeModel *mUelTreeModel;
    QSortFilterProxyModel *mUelFilterModel;
};

}
}
}

#endif // GLOBALFILTERDIALOG_H
