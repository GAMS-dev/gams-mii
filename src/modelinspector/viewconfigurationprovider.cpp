#include "viewconfigurationprovider.h"
#include "abstractmodelinstance.h"

#include <QAbstractItemModel>
#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

class DefaultViewConfiguration final : public AbstractViewConfiguration
{
public:
    DefaultViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {

    }

    AbstractViewConfiguration* clone() override
    {
        return new DefaultViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        Q_UNUSED(orientation);
        return IdentifierStates();
    }
};

class JaccobianViewConfiguration final : public AbstractViewConfiguration
{
public:
    JaccobianViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(AbstractViewConfiguration::viewType());
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(AbstractViewConfiguration::viewType());
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
    }

    AbstractViewConfiguration* clone() override
    {
        return new JaccobianViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }
};

class BPScalingViewConfiguration final : public AbstractViewConfiguration
{
public:
    BPScalingViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        setSectionLabels();
    }

    AbstractViewConfiguration* clone() override
    {
        return new BPScalingViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }

private:
    void setSectionLabels()
    {
        int currentRow = 0;
        mVerticalSectionLabels.clear();
        for (auto* equation : mModelInstance->equations()) {
            symbolLabels(equation->name(), currentRow);
            currentRow = mVerticalSectionLabels.size();
        }
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"Variable", "Max"};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"", "Min"};
        int index = 0;
        for (auto var : mModelInstance->variables()) {
            mHorizontalSectionLabels[index++] = QStringList(var->name());
        }
        mHorizontalSectionLabels[index++] = QStringList("RHS");
        mHorizontalSectionLabels[index++] = QStringList("Equation");
    }

    void symbolLabels(const QString &symName, int currentRow)
    {
        for (int s=currentRow; s<currentRow+2; ++s) {
            auto data = mVerticalSectionLabels[s];
            if (s % 2) {
                data << QString() << "Min";
                mVerticalSectionLabels[s] = data;
            } else {
                data << symName << "Max";
                mVerticalSectionLabels[s] = data;
            }
        }
    }
};

class SymbolViewConfiguration final : public AbstractViewConfiguration
{
public:
    SymbolViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        mDefaultAggregation.setViewType(this->viewType());
        mCurrentAggregation.setViewType(this->viewType());
    }

    AbstractViewConfiguration* clone() override
    {
        return new SymbolViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }
};

class BPOverviewViewConfiguration final : public AbstractViewConfiguration
{
public:
    BPOverviewViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultValueFilter.MinValue = mModelInstance->modelMinimum(AbstractViewConfiguration::viewType());
        mDefaultValueFilter.MaxValue = mModelInstance->modelMaximum(AbstractViewConfiguration::viewType());
        mCurrentValueFilter = mDefaultValueFilter;
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        setSectionLabels();
    }

    AbstractViewConfiguration* clone() override
    {
        return new BPOverviewViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }

private:
    void setSectionLabels()
    {
        int index = 0;
        for (auto eqn : mModelInstance->equations()) {
            mVerticalSectionLabels[index++] = QStringList(eqn->name());
        }
        mVerticalSectionLabels[index++] = QStringList("Variable Type");
        index = 0;
        for (auto var : mModelInstance->variables()) {
            mHorizontalSectionLabels[index++] = QStringList(var->name());
        }
        mHorizontalSectionLabels[index++] = QStringList("Type");
        mHorizontalSectionLabels[index++] = QStringList("RHS");
    }
};

class BPCountViewConfiguration final : public AbstractViewConfiguration
{
public:
    BPCountViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        setSectionLabels();
    }

    AbstractViewConfiguration* clone() override
    {
        return new BPCountViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }

private:
    void setSectionLabels()
    {
        int currentRow = 0;
        mVerticalSectionLabels.clear();
        for (auto* equation : mModelInstance->equations()) {
            symbolLabels(equation->name(), currentRow);
            currentRow = mVerticalSectionLabels.size();
        }
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"Coeff Cnts", "Pos"};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"", "Neg"};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"# of Vars", ""};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"Variable Type", ""};
        int index = 0;
        for (auto var : mModelInstance->variables()) {
            mHorizontalSectionLabels[index++] = QStringList(var->name());
        }
        mHorizontalSectionLabels[index++] = QStringList("Type");
        mHorizontalSectionLabels[index++] = QStringList("RHS");
        mHorizontalSectionLabels[index++] = QStringList("Coeff Cnts");
        mHorizontalSectionLabels[index++] = QStringList("# of Eqns");
    }

    void symbolLabels(const QString &symName, int currentRow)
    {
        for (int s=currentRow; s<currentRow+2; ++s) {
            auto data = mVerticalSectionLabels[s];
            if (s % 2) {
                data << QString() << "Neg";
                mVerticalSectionLabels[s] = data;
            } else {
                data << symName << "Pos";
                mVerticalSectionLabels[s] = data;
            }
        }
    }
};

class BPAverageViewConfiguration final : public AbstractViewConfiguration
{
public:
    BPAverageViewConfiguration(ViewDataType viewType, QSharedPointer<AbstractModelInstance> modelInstance)
        : AbstractViewConfiguration(viewType, modelInstance)
    {
        createLabelFilter();
        mDefaultIdentifierFilter[Qt::Horizontal] = createDefaultSymbolFilter(Qt::Horizontal);
        mDefaultIdentifierFilter[Qt::Vertical] = createDefaultSymbolFilter(Qt::Vertical);
        mCurrentIdentifierFilter = mDefaultIdentifierFilter;
        setSectionLabels();
    }

    AbstractViewConfiguration* clone() override
    {
        return new BPAverageViewConfiguration(*this);
    }

protected:
    IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const override
    {
        const auto& symbols = orientation == Qt::Horizontal ? mModelInstance->variables()
                                                            : mModelInstance->equations();
        int symIndex = 0;
        IdentifierStates states;
        for (const auto& sym : symbols) {
            IdentifierState identifierState;
            identifierState.Enabled = true;
            identifierState.SectionIndex = symIndex;
            identifierState.SymbolIndex = sym->firstSection();
            identifierState.Text = sym->name();
            identifierState.Checked = Qt::Checked;
            states[sym->firstSection()] = identifierState;
            symIndex++;
        }
        return states;
    }

private:
    void setSectionLabels()
    {
        int currentRow = 0;
        mVerticalSectionLabels.clear();
        for (auto* equation : mModelInstance->equations()) {
            symbolLabels(equation->name(), currentRow);
            currentRow = mVerticalSectionLabels.size();
        }
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"Cfs PerVar", "Pos"};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"", "Neg"};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"# of Vars", ""};
        mVerticalSectionLabels[mVerticalSectionLabels.size()] = QStringList {"Variable Type", ""};
        int index = 0;
        for (auto var : mModelInstance->variables()) {
            mHorizontalSectionLabels[index++] = QStringList(var->name());
        }
        mHorizontalSectionLabels[index++] = QStringList("Type");
        mHorizontalSectionLabels[index++] = QStringList("RHS");
        mHorizontalSectionLabels[index++] = QStringList("Cfs PerEqu");
        mHorizontalSectionLabels[index++] = QStringList("# of Eqns");
    }

    void symbolLabels(const QString &symName, int currentRow)
    {
        for (int s=currentRow; s<currentRow+2; ++s) {
            auto data = mVerticalSectionLabels[s];
            if (s % 2) {
                data << QString() << "Neg";
                mVerticalSectionLabels[s] = data;
            } else {
                data << symName << "Pos";
                mVerticalSectionLabels[s] = data;
            }
        }
    }
};

AbstractViewConfiguration::AbstractViewConfiguration(ViewDataType viewType,
                                                     QSharedPointer<AbstractModelInstance> modelInstance)
    : mModelInstance(modelInstance)
    , mView((int)viewType)
    , mViewType(viewType)
{

}

void AbstractViewConfiguration::setModelInstance(QSharedPointer<AbstractModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
}

void AbstractViewConfiguration::updateIdentifierFilter(const QList<Symbol *> &eqnFilter,
                                                       const QList<Symbol *> &varFilter)
{
    for (auto iter=mCurrentIdentifierFilter[Qt::Vertical].begin();
         iter!=mCurrentIdentifierFilter[Qt::Vertical].end(); ++iter) {
        for (auto symbol : eqnFilter) {
            if (iter->Text == symbol->name()) {
                iter->Checked = Qt::Checked;
                break;
            } else {
                iter->Checked = Qt::Unchecked;
            }
        }
    }
    for (auto iter=mCurrentIdentifierFilter[Qt::Horizontal].begin();
         iter!=mCurrentIdentifierFilter[Qt::Horizontal].end(); ++iter) {
        for (auto symbol : varFilter) {
            if (iter->Text == symbol->name()) {
                iter->Checked = Qt::Checked;
                break;
            } else {
                iter->Checked = Qt::Unchecked;
            }
        }
    }
    mDefaultIdentifierFilter = mCurrentIdentifierFilter;
}

const SectionLabels &AbstractViewConfiguration::sectionLabels(Qt::Orientation orientation) const
{
    return orientation == Qt::Horizontal ? mHorizontalSectionLabels : mVerticalSectionLabels;
}

void AbstractViewConfiguration::createLabelFilter()
{
    for (const auto& label : mModelInstance->labels()) {
        mDefaultLabelFilter.LabelCheckStates[Qt::Horizontal][label] = Qt::Checked;
        mDefaultLabelFilter.LabelCheckStates[Qt::Vertical][label] = Qt::Checked;
        mCurrentLabelFilter.LabelCheckStates[Qt::Horizontal][label] = Qt::Checked;
        mCurrentLabelFilter.LabelCheckStates[Qt::Vertical][label] = Qt::Checked;
    }
}

AbstractViewConfiguration *ViewConfigurationProvider::defaultConfiguration()
{
    auto modelInstance = QSharedPointer<AbstractModelInstance>(new EmptyModelInstance);
    return new DefaultViewConfiguration(ViewDataType::Unknown, modelInstance);
}

AbstractViewConfiguration *ViewConfigurationProvider::configuration(ViewDataType viewType,
                                                                    QSharedPointer<AbstractModelInstance> modelInstance)
{
    switch (viewType) {
    case ViewDataType::BP_Scaling:
        return new BPScalingViewConfiguration(viewType, modelInstance);
    case ViewDataType::Jaccobian:
        return new JaccobianViewConfiguration(viewType, modelInstance);
    case ViewDataType::Symbols:
        return new SymbolViewConfiguration(viewType, modelInstance);
    case ViewDataType::BP_Overview:
        return new BPOverviewViewConfiguration(viewType, modelInstance);
    case ViewDataType::BP_Count:
        return new BPCountViewConfiguration(viewType, modelInstance);
    case ViewDataType::BP_Average:
        return new BPAverageViewConfiguration(viewType, modelInstance);
    default:
        return new DefaultViewConfiguration(viewType, modelInstance);
    }
}

}
}
}
