#ifndef GLOBALFILTERDIALOG_H
#define GLOBALFILTERDIALOG_H

#include <QDialog>

#include "common.h"

class QSortFilterProxyModel;
class QStandardItem;
class QTreeView;

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

    SymbolFilterMap symbolFilter() const;
    void setSymbolFilter(const SymbolFilterMap &filter);
    void setDefaultSymbolFilter(const SymbolFilterMap &filter);

    ValueFilter valueFilter() const;
    void setValueFilter(const ValueFilter &filter);
    void setDefaultValueFilter(const ValueFilter &filter);

    UelFilterMap uelFilter() const;
    void setUelFilter(const UelFilterMap &filter);
    void setDefaultUelFilter(const UelFilterMap &filter);

signals:
    void filterUpdated();

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

private:
    void setupEquationFilter(const SymbolFilter &filter);
    void setupVariableFilter(const SymbolFilter &filter);
    void setupUelFilter();
    FilterTreeItem* setupSymTreeItems(Qt::Orientation orientation, const SymbolFilter &filter);
    void setupUelTreeItems(Qt::Orientation orientation, FilterTreeItem *root);

    void applyCheckState(QTreeView* view,
                         QSortFilterProxyModel *model,
                         Qt::CheckState state);
    SymbolFilter applyHeaderFilter(QSortFilterProxyModel *model);
    void applyValueFilter();
    UelFilter applyUelFilter(Qt::Orientation orientation,
                             QSortFilterProxyModel *model);

private:
    Ui::GlobalFilterDialog *ui;

    SymbolFilterMap mSymbolFilter;
    SymbolFilterMap mDefaultSymbolFilter;
    ValueFilter mValueFilter;
    ValueFilter mDefaultValueFilter;
    UelFilterMap mUelFilter;
    UelFilterMap mDefaultUelFilter;

    QSortFilterProxyModel *mEqnFilterModel;
    QSortFilterProxyModel *mVarFilterModel;
    QSortFilterProxyModel *mUelFilterModel;
};

}
}
}

#endif // GLOBALFILTERDIALOG_H
