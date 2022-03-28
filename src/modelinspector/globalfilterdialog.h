#ifndef GLOBALFILTERDIALOG_H
#define GLOBALFILTERDIALOG_H

#include <QDialog>

#include "common.h"

class QSortFilterProxyModel;
class QLineEdit;
class QTreeView;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class GlobalFilterDialog;
}

class FilterTreeItem;

class GlobalFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalFilterDialog(QWidget *parent = nullptr);
    ~GlobalFilterDialog();

    IdentifierFilter idendifierFilter() const;
    void setIdentifierFilter(const IdentifierFilter &filter);
    void setDefaultIdentifierFilter(const IdentifierFilter &filter);

    ValueFilter valueFilter() const;
    void setValueFilter(const ValueFilter &filter);
    void setDefaultValueFilter(const ValueFilter &filter);

    LabelFilter labelFilter() const;
    void setLabelFilter(const LabelFilter &filter);
    void setDefaultLabelFilter(const LabelFilter &filter);

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

    void on_selectLabelButton_clicked();

    void on_deselectLabelButton_clicked();

    void on_labelFilterBox_currentIndexChanged(int index);

private:
    void setupEquationFilter(const IdentifierStates &filter);
    void setupVariableFilter(const IdentifierStates &filter);
    void setupLabelFilter();
    FilterTreeItem* setupSymTreeItems(Qt::Orientation orientation, const IdentifierStates &filter);
    void setupLabelTreeItems(Qt::Orientation orientation, FilterTreeItem *root);

    void applyCheckState(QTreeView* view,
                         QSortFilterProxyModel *model,
                         Qt::CheckState state);
    IdentifierStates applyHeaderFilter(QSortFilterProxyModel *model);
    void applyValueFilter();
    LabelStates applyLabelFilter(Qt::Orientation orientation,
                                QSortFilterProxyModel *model);

    void updateRangeEdit(QLineEdit *edit, const QString &text);

private:
    Ui::GlobalFilterDialog *ui;

    IdentifierFilter mIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;
    ValueFilter mValueFilter;
    ValueFilter mDefaultValueFilter;
    LabelFilter mLabelFilter;
    LabelFilter mDefaultLabelFilter;

    QSortFilterProxyModel *mEqnFilterModel;
    QSortFilterProxyModel *mVarFilterModel;
    QSortFilterProxyModel *mLabelFilterModel;
};

}
}
}

#endif // GLOBALFILTERDIALOG_H
