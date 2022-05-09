#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

#include "common.h"

class QSortFilterProxyModel;
class QLineEdit;
class QTreeView;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class FilterDialog;
}

class FilterTreeItem;

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

    IdentifierFilter idendifierFilter() const;
    void setIdentifierFilter(const IdentifierFilter &filter);
    void setDefaultIdentifierFilter(const IdentifierFilter &filter);

    ValueFilter valueFilter() const;
    void setValueFilter(const ValueFilter &filter);
    void setDefaultValueFilter(const ValueFilter &filter);

    LabelFilter labelFilter() const;
    void setLabelFilter(const LabelFilter &filter);
    void setDefaultLabelFilter(const LabelFilter &filter);

    DataSource horizontalDataSource() const;
    void setHorizontalDataSource(DataSource dataSource);

    DataSource verticalDataSource() const;
    void setVerticalDataSource(DataSource dataSource);

    PredefinedViewEnum viewType() const;
    void setViewType(PredefinedViewEnum viewType);

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

    void on_labelBox_currentIndexChanged(int index);

private:
    void setupEquationFilter(const IdentifierStates &filter);
    void setupVariableFilter(const IdentifierStates &filter);
    void setupLabelFilter();
    FilterTreeItem* setupSymTreeItems(const QString &text,
                                      const IdentifierStates &filter,
                                      bool showAttributes,
                                      bool showSymbols);
    void setupLabelTreeItems(const QString &text, Qt::Orientation orientation, FilterTreeItem *root);

    void applyCheckState(QTreeView* view,
                         QSortFilterProxyModel *model,
                         Qt::CheckState state);
    IdentifierStates applyHeaderFilter(QSortFilterProxyModel *model,
                                       DataSource dataSource);
    void applyValueFilter();
    LabelCheckStates applyLabelFilter(Qt::Orientation orientation,
                                      QSortFilterProxyModel *model);

    void updateRangeEdit(QLineEdit *edit, const QString &text);

    void disableAttributes(QSortFilterProxyModel *model);

    Qt::Orientation equationOrientation() const {
        return mVerticalDataSource == DataSource::EquationData ? Qt::Vertical : Qt::Horizontal;
    }

    Qt::Orientation variableOrientation() const {
        return mHorizontalDataSource == DataSource::VariableData ? Qt::Horizontal : Qt::Vertical;
    }

private:
    Ui::FilterDialog *ui;

    PredefinedViewEnum mViewType;

    IdentifierFilter mIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;
    ValueFilter mValueFilter;
    ValueFilter mDefaultValueFilter;
    LabelFilter mLabelFilter;
    LabelFilter mDefaultLabelFilter;

    QSortFilterProxyModel *mEqnFilterModel;
    QSortFilterProxyModel *mVarFilterModel;
    QSortFilterProxyModel *mLabelFilterModel;

    DataSource mHorizontalDataSource;
    DataSource mVerticalDataSource;
};

}
}
}

#endif // FILTERDIALOG_H
