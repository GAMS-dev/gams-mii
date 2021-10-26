#ifndef GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
#define GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H

#include <QWidget>

class QSortFilterProxyModel;
class QStandardItem;

namespace gams {
namespace studio {
namespace modelinspector {

namespace Ui {
class LabelFilterWidget;
}

class FilterTreeItem;
class FilterTreeModel;

class LabelFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LabelFilterWidget(Qt::Orientation orientation,
                               QWidget *parent = nullptr);
    ~LabelFilterWidget();

    void setData(FilterTreeItem *rootItem);

protected:
    void showEvent(QShowEvent *event);

signals:
    void filterChanged(FilterTreeItem *item, Qt::Orientation orientation);

private slots:
    void on_applyButton_clicked();

    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void applyFilter(const QString &text);

private:
    void setSelection(bool state);

private:
    Ui::LabelFilterWidget *ui;
    Qt::Orientation mOrientation;
    FilterTreeItem *mRootItem;
    QSortFilterProxyModel *mFilterModel;
};


} // namespace modelinspector
} // namespace studio
} // namespace gams
#endif // GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
