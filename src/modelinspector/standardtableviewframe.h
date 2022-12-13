#ifndef STANDARDTABLEVIEWFRAME_H
#define STANDARDTABLEVIEWFRAME_H

#include "abstracttableviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

class EquationAttributeTableModel;
class VariableAttributeTableModel;
class ColumnRowFilterModel;
class IdentifierFilterModel;
class IdentifierLabelFilterModel;
class ValueFormatProxyModel;
class LabelFilterModel;
class AggregationProxyModel;
class HierarchicalHeaderView;
class JaccobianTableModel;
class ModelInstanceTableModel;

class AbstractStandardTableViewFrame : public AbstractTableViewFrame
{
    Q_OBJECT

public:
    AbstractStandardTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractStandardTableViewFrame() = default;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    const Aggregation& appliedAggregation() const override;

    void setAggregation(const Aggregation &aggregation, int view) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter) override;

    void reset(PredefinedViewEnum view) override;

    void updateView() override;

protected:
    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation);

    void cloneFilterAndAggregation(AbstractStandardTableViewFrame *clone, int newView);

    void updateValueFilter() override;

protected:
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    IdentifierFilterModel* mIdentifierFilterModel = nullptr;
    IdentifierLabelFilterModel* mIdentifierLabelFilterModel = nullptr;
    LabelFilterModel* mLabelFilterModel = nullptr;
    AggregationProxyModel* mAggregationModel = nullptr;
    ColumnRowFilterModel* mColumnRowFilterModel = nullptr;
};

class EqnTableViewFrame : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    EqnTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::EqnAttributes;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

protected:
    Aggregation getDefaultAggregation() const override;

private slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<EquationAttributeTableModel> mBaseModel;
};

class VarTableViewFrame : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    VarTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::VarAttributes;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter) override;

protected:
    Aggregation getDefaultAggregation() const override;

private slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<VariableAttributeTableModel> mBaseModel;
};

class JaccTableViewFrame : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    JaccTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::Jaccobian;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

public slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<JaccobianTableModel> mBaseModel;
};

class FullTableViewFrame : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    FullTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::Full;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

public slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<ModelInstanceTableModel> mModelInstanceModel;
};

}
}
}

#endif // STANDARDTABLEVIEWFRAME_H
