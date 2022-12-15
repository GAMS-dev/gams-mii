#ifndef MINMAXTABLEVIEWFRAME_H
#define MINMAXTABLEVIEWFRAME_H

#include "abstracttableviewframe.h"

class QMenu;

namespace gams {
namespace studio{
namespace modelinspector {

class ColumnRowFilterModel;
class HierarchicalHeaderView;
class MinMaxIdentifierFilterModel;
class MinMaxModelInstanceTableModel;
class ValueFormatProxyModel;

class MinMaxTableViewFrame : public AbstractTableViewFrame
{
    Q_OBJECT

public:
    MinMaxTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::MinMax;
    }

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    const Aggregation& appliedAggregation() const override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) override;

    void setValueFilter(const ValueFilter &filter) override;

    void setAggregation(const Aggregation &aggregation, int view) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter) override;

    QList<Symbol> selectedEquations() const;
    QList<Symbol> selectedVariables() const;

    void reset(int view) override;

    void updateView() override;

public slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private slots:
    void customMenuRequested(const QPoint &pos);

protected:
    Aggregation getDefaultAggregation() const override;

    Aggregation appliedAggregation(const Aggregation &aggregation, int view) const override;

private:
    void setupSelectionMenu();
    void handleRowColumnSelection(PredefinedViewEnum type);
    void updateValueFilter() override;
    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation);
    void cloneFilterAndAggregation(MinMaxTableViewFrame *clone, int newView);

private:
    QSharedPointer<MinMaxModelInstanceTableModel> mModelInstanceModel;
    QMenu *mSelectionMenu;

    QList<Symbol> mSelectedEquations;
    QList<Symbol> mSelectedVariables;

    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    MinMaxIdentifierFilterModel* mIdentifierFilterModel = nullptr;
    ColumnRowFilterModel* mColumnRowFilterModel = nullptr;
};

}
}
}

#endif // MINMAXTABLEVIEWFRAME_H
