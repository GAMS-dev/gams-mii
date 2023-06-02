#ifndef BPVIEWFRAME_H
#define BPVIEWFRAME_H

#include "standardtableviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

class BPOverviewTableModel;
class BPScalingIdentifierFilterModel;
class ComprehensiveTableModel;
class HierarchicalHeaderView;
class ComprehensiveTableModel;
class ValueFormatProxyModel;

class BPOverviewViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BPOverviewViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BPOverviewViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                              QSharedPointer<AbstractViewConfiguration> viewConfig,
                              QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame *clone(int view) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    inline ViewDataType type() const override
    {
        return ViewDataType::BP_Overview;
    }

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void setupView();

private:
    QSharedPointer<BPOverviewTableModel> mBaseModel;
};

class BPCountViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BPCountViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BPCountViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                           QSharedPointer<AbstractViewConfiguration> viewConfig,
                           QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame *clone(int view) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    inline ViewDataType type() const override
    {
        return ViewDataType::BP_Count;
    }

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void setupView();

private:
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    QSharedPointer<ComprehensiveTableModel> mBaseModel;
};

class BPAverageViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BPAverageViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BPAverageViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                             QSharedPointer<AbstractViewConfiguration> viewConfig,
                             QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame *clone(int view) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    inline ViewDataType type() const override
    {
        return ViewDataType::BP_Average;
    }

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void setupView();

private:
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    QSharedPointer<ComprehensiveTableModel> mBaseModel;
};

class BPScalingViewFrame final : public AbstractTableViewFrame
{
    Q_OBJECT

public:
    BPScalingViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BPScalingViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                       QSharedPointer<AbstractViewConfiguration> viewConfig,
                       QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    ViewDataType type() const override
    {
        return ViewDataType::BP_Scaling;
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
    void setupView();
    void handleRowColumnSelection();
    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation);

private:
    QSharedPointer<ComprehensiveTableModel> mBaseModel;
    QMenu *mSelectionMenu;
    QAction *mSymbolAction = new QAction("Show selected symbols", this);

    QList<Symbol*> mSelectedEquations;
    QList<Symbol*> mSelectedVariables;

    HierarchicalHeaderView* mVerticalHeader = nullptr;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    BPScalingIdentifierFilterModel* mIdentifierFilterModel = nullptr;
};


}
}
}

#endif // BPVIEWFRAME_H
