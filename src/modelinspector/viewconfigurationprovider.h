#ifndef VIEWCONFIGURATIONPROVIDER_H
#define VIEWCONFIGURATIONPROVIDER_H

#include "aggregation.h"
#include "common.h"

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class AbstractViewConfiguration
{
public:
    AbstractViewConfiguration(ViewDataType viewType,
                              QSharedPointer<AbstractModelInstance> modelInstance = nullptr);

    virtual ~AbstractViewConfiguration() {}

    virtual AbstractViewConfiguration* clone() = 0;

    virtual void initialize(QAbstractItemModel *model) = 0;

    void setModelInstance(QSharedPointer<AbstractModelInstance> modelInstance);

    inline int view() const {
        return mView;
    }

    inline void setView(int view) {
        mView = view;
    }

    Aggregation& currentAggregation() {
        return mCurrentAggregation;
    }

    void setCurrentAggregation(const Aggregation& aggregation) {
        mCurrentAggregation = aggregation;
    }

    const Aggregation& defaultAggregation() const {
        return mDefaultAggregation;
    }

    void setDefaultAggregation(const Aggregation& aggregation) {
        mDefaultAggregation = aggregation;
    }

    const LabelFilter& currentLabelFiler() const {
        return mCurrentLabelFilter;
    }

    void setCurrentLabelFilter(const LabelFilter& filter) {
        mCurrentLabelFilter = filter;
    }

    const LabelFilter& defaultLabelFilter() const {
        return mDefaultLabelFilter;
    }

    void setDefaultLabelFilter(const LabelFilter& filter) {
        mDefaultLabelFilter = filter;
    }

    IdentifierFilter& currentIdentifierFilter() {
        return mCurrentIdentifierFilter;
    }

    void setCurrentIdentifierFilter(const IdentifierFilter& filter) {
        mCurrentIdentifierFilter = filter;
    }

    const IdentifierFilter& defaultIdentifierFilter() const {
        return mDefaultIdentifierFilter;
    }

    void setDefaultIdentifierFilter(const IdentifierFilter& filter) {
        mDefaultIdentifierFilter = filter;
    }

    void updateIdentifierFilter(const QList<Symbol *> &eqnFilter,
                                const QList<Symbol *> &varFilter);

    ValueFilter& currentValueFilter() {
        return mCurrentValueFilter;
    }

    void setCurrentValueFilter(const ValueFilter& filter) {
        mCurrentValueFilter = filter;
    }

    ValueFilter& defaultValueFilter() {
        return mDefaultValueFilter;
    }

    void setDefaultValueFilter(const ValueFilter& filter) {
        mDefaultValueFilter = filter;
    }

protected:
    virtual IdentifierStates createDefaultSymbolFilter(Qt::Orientation orientation) const = 0;

    virtual void createLabelFilter();

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    ViewDataType mViewType;
    int mView;

    LabelFilter mCurrentLabelFilter;
    LabelFilter mDefaultLabelFilter;

    IdentifierFilter mCurrentIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;

    ValueFilter mCurrentValueFilter;
    ValueFilter mDefaultValueFilter;

    Aggregation mCurrentAggregation;
    Aggregation mDefaultAggregation;
};

class ViewConfigurationProvider final
{
private:
    ViewConfigurationProvider() {};

public:
    static AbstractViewConfiguration* configuration(ViewDataType viewType,
                                                    QSharedPointer<AbstractModelInstance> modelInstance = nullptr);
};

}
}
}

#endif // VIEWCONFIGURATIONPROVIDER_H
