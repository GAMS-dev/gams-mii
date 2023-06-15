#ifndef ABSTRACTVIEWFRAME_H
#define ABSTRACTVIEWFRAME_H

#include "common.h"

#include <QFrame>

namespace gams {
namespace studio {
namespace modelinspector {

class Aggregation;
class AbstractModelInstance;
class AbstractViewConfiguration;

class AbstractViewFrame : public QFrame
{
    Q_OBJECT

public:
    AbstractViewFrame(QWidget *parent = nullptr,
                      Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractViewFrame();

    virtual AbstractViewFrame* clone(int view) = 0;

    const IdentifierFilter& identifierFilter() const;

    const IdentifierFilter& defaultIdentifierFilter() const;

    virtual void setIdentifierFilter(const IdentifierFilter &filter) = 0;

    void setDefaultIdentifierFilter(const IdentifierFilter &filter);

    const ValueFilter& valueFilter() const;

    const ValueFilter& defaultValueFilter() const;

    virtual void setValueFilter(const ValueFilter &filter);

    void setDefaultValueFilter(const ValueFilter &filter);

    const LabelFilter& labelFilter() const;

    const LabelFilter& defaultLabelFilter() const;

    virtual void setLabelFilter(const LabelFilter &filter);

    void setDefaultLabelFilter(const LabelFilter &filter);

    virtual void setAggregation(const Aggregation &aggregation) = 0;

    void setDefaultAggregation(const Aggregation& aggregation);

    const Aggregation& currentAggregation() const;

    const Aggregation& defaultAggregation() const;

    virtual void setShowAbsoluteValues(bool absoluteValues) = 0;

    virtual ViewDataType type() const = 0;

    virtual void reset() = 0;

    virtual void updateView() = 0;

    virtual void zoomIn() = 0;

    virtual void zoomOut() = 0;

    virtual void resetZoom() = 0;

    virtual SearchResult& search(const QString &term, bool isRegEx) = 0;

    SearchResult& searchResult();

    virtual void setSearchSelection(const SearchResult::SearchEntry &result) = 0;

    int view() const;

    virtual void setView(int view);

    virtual void setupView(QSharedPointer<AbstractModelInstance> modelInstance) = 0;

    QSharedPointer<AbstractViewConfiguration> viewConfig() const;

    void setViewConfig(QSharedPointer<AbstractViewConfiguration> viewConfig);

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
};

}
}
}

#endif // ABSTRACTVIEWFRAME_H
