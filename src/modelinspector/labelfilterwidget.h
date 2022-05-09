#ifndef GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
#define GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H

#include <QWidget>

#include "common.h"

class QSortFilterProxyModel;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class LabelFilterWidget;
}

class FilterTreeItem;

class LabelFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LabelFilterWidget(Qt::Orientation orientation,
                               QWidget *parent = nullptr);
    ~LabelFilterWidget();

    void setData(FilterTreeItem *rootItem);

    void setSymbolType(DataSource type);

protected:
    void showEvent(QShowEvent *event);

signals:
    void filterChanged(const gams::studio::modelinspector::IdentifierState&, Qt::Orientation);

private slots:
    void on_applyButton_clicked();

    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void applyFilter(const QString &text);

private:
    void applyCheckState(bool state);
    IdentifierState identifierState();

private:
    Ui::LabelFilterWidget *ui;
    Qt::Orientation mOrientation;
    QSortFilterProxyModel *mFilterModel;
    DataSource mSymbolType;
};


} // namespace modelinspector
} // namespace studio
} // namespace gams
#endif // GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
