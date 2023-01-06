#ifndef MINMAXTABLEVIEWFRAME_H
#define MINMAXTABLEVIEWFRAME_H

#include "abstracttableviewframe.h"

class QMenu;

namespace gams {
namespace studio{
namespace modelinspector {

class HierarchicalHeaderView;
class MinMaxIdentifierFilterModel;
class MinMaxModelInstanceTableModel;
class ValueFormatProxyModel;

class MinMaxTableViewFrame final : public AbstractTableViewFrame
{
    Q_OBJECT

public:
    MinMaxTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void aggregate(QSharedPointer<AbstractModelInstance> modelInstance);

    AbstractTableViewFrame* clone(int view) override;

    ViewDataType type() const override
    {
        return ViewDataType::MinMax;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void setAggregation(const Aggregation &aggregation) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    const QList<Symbol*>& selectedEquations() const;
    const QList<Symbol*>& selectedVariables() const;

    void reset() override;

    void updateView() override;

signals:
    void newSymbolViewRequested();

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private slots:
    void customMenuRequested(const QPoint &pos);

private:
    void setupSelectionMenu();
    void handleRowColumnSelection();
    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation);

private:
    QSharedPointer<MinMaxModelInstanceTableModel> mModelInstanceModel;
    QMenu *mSelectionMenu;

    QList<Symbol*> mSelectedEquations;
    QList<Symbol*> mSelectedVariables;

    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    MinMaxIdentifierFilterModel* mIdentifierFilterModel = nullptr;
};

}
}
}

#endif // MINMAXTABLEVIEWFRAME_H
