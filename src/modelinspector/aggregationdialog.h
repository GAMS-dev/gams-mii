#ifndef AGGREGATIONDIALOG_H
#define AGGREGATIONDIALOG_H

#include <QDialog>

#include "common.h"

#include "aggregation.h"

class QSortFilterProxyModel;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class AggregationDialog;
}

class FilterTreeItem;

class AggregationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AggregationDialog(QWidget *parent = nullptr);
    ~AggregationDialog();

    const Aggregation& aggregation() const;
    void setAggregation(const Aggregation &aggregation, const IdentifierFilter &filter);
    void setDefaultAggregation(const Aggregation &aggregation);

signals:
    void aggregationUpdated();

public slots:
    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void on_cancelButton_clicked();

    void on_resetButton_clicked();

    void on_applyButton_clicked();

private slots:
    void filterUpdate(const QString &text);

private:
    void setupAggregationView();
    void setupTreeItems(Qt::Orientation orientation, FilterTreeItem *root);
    void setAggregationMethodAvailability();

    void applyAggregation();
    AggregationSymbols checkStates(FilterTreeItem *item);
    void applyCheckState(bool state);

private:
    Ui::AggregationDialog *ui;
    int mAggregationMethod = 0;

    Aggregation mAggregation;
    Aggregation mDefaultAggregation;
    IdentifierFilter mIdentifierFilter;

    QSortFilterProxyModel *mAggregationModel = nullptr;
};

}
}
}

#endif // AGGREGATIONDIALOG_H
