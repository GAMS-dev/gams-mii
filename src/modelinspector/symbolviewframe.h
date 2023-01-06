#ifndef SYMBOLVIEWFRAME_H
#define SYMBOLVIEWFRAME_H

#include "standardtableviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

class SymbolModelInstanceTableModel;

class SymbolViewFrame final : public AbstractStandardTableViewFrame
{
    Q_OBJECT

public:
    SymbolViewFrame(int view, QSharedPointer<AbstractModelInstance> modelInstance,
                    QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void aggregate();

    AbstractTableViewFrame *clone(int view) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    ViewDataType type() const override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    void setView(int view) override;

    void updateView() override;

protected slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    QSharedPointer<SymbolModelInstanceTableModel> mModelInstanceModel;
    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
};

}
}
}

#endif // SYMBOLVIEWFRAME_H
