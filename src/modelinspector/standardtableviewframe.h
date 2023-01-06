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

class AbstractStandardTableViewFrame : public AbstractTableViewFrame
{
    Q_OBJECT

public:
    AbstractStandardTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractStandardTableViewFrame() = default;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setAggregation(const Aggregation &aggregation) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    void reset() override;

    void updateView() override;

protected:
    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation);

    void cloneFilterAndAggregation(AbstractStandardTableViewFrame *clone, int newView);

protected:
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    IdentifierFilterModel* mIdentifierFilterModel = nullptr;
    IdentifierLabelFilterModel* mIdentifierLabelFilterModel = nullptr;
    LabelFilterModel* mLabelFilterModel = nullptr;
    AggregationProxyModel* mAggregationModel = nullptr;
    ColumnRowFilterModel* mColumnRowFilterModel = nullptr;
};

class EqnTableViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    EqnTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    ViewDataType type() const override
    {
        return ViewDataType::EqnAttributes;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<EquationAttributeTableModel> mBaseModel;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
};

class VarTableViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    VarTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    ViewDataType type() const override
    {
        return ViewDataType::VarAttributes;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<VariableAttributeTableModel> mBaseModel;
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
};

class JaccTableViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    JaccTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    ViewDataType type() const override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<JaccobianTableModel> mBaseModel;
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
};

}
}
}

#endif // STANDARDTABLEVIEWFRAME_H
