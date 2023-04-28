#ifndef BLOCKPICVIEWFRAME_H
#define BLOCKPICVIEWFRAME_H

#include "standardtableviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

class BlockpicOverviewTableModel;
class ComprehensiveTableModel;

class BlockpicOverviewViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BlockpicOverviewViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BlockpicOverviewViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
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
    QSharedPointer<BlockpicOverviewTableModel> mBaseModel;
};

class BlockpicCountViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BlockpicCountViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BlockpicCountViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
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

class BlockpicAverageViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    BlockpicAverageViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    BlockpicAverageViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
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

}
}
}

#endif // BLOCKPICVIEWFRAME_H
