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

protected:
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    IdentifierFilterModel* mIdentifierFilterModel = nullptr;
    IdentifierLabelFilterModel* mIdentifierLabelFilterModel = nullptr;
    LabelFilterModel* mLabelFilterModel = nullptr;
};

class JaccTableViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    JaccTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    JaccTableViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                       QSharedPointer<AbstractViewConfiguration> viewConfig,
                       QWidget *parent = nullptr,
                       Qt::WindowFlags f = Qt::WindowFlags());

    AbstractTableViewFrame* clone(int view) override;

    inline ViewDataType type() const override
    {
        return ViewDataType::Jaccobian;
    }

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void setupView();

private:
    QSharedPointer<JaccobianTableModel> mBaseModel;
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
};

}
}
}

#endif // STANDARDTABLEVIEWFRAME_H
