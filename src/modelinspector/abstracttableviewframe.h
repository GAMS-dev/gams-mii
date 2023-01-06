#ifndef ABSTRACTTABLEVIEWFRAME_H
#define ABSTRACTTABLEVIEWFRAME_H

#include "common.h"
#include "aggregation.h"

#include <QFrame>
#include <QSharedPointer>

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class StandardTableViewFrame;
}

class AbstractModelInstance;
class AbstractViewConfiguration;

class AbstractTableViewFrame : public QFrame
{
    Q_OBJECT

public:
    AbstractTableViewFrame(QWidget *parent = nullptr,
                           Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractTableViewFrame();

    virtual AbstractTableViewFrame* clone(int view) = 0;

    virtual const IdentifierFilter& identifierFilter() const;

    virtual const IdentifierFilter& defaultIdentifierFilter() const;

    virtual void setIdentifierFilter(const IdentifierFilter &filter) = 0;

    void setDefaultIdentifierFilter(const IdentifierFilter &filter);

    virtual const ValueFilter& valueFilter() const;

    virtual const ValueFilter& defaultValueFilter() const;

    virtual void setValueFilter(const ValueFilter &filter);

    void setDefaultValueFilter(const ValueFilter &filter);

    virtual const LabelFilter& labelFilter() const;

    virtual const LabelFilter& defaultLabelFilter() const;

    virtual void setLabelFilter(const LabelFilter &filter);

    void setDefaultLabelFilter(const LabelFilter &filter);

    virtual const Aggregation& currentAggregation() const;

    virtual const Aggregation& defaultAggregation() const;

    virtual void setAggregation(const Aggregation &aggregation) = 0;

    void setDefaultAggregation(const Aggregation& aggregation);

    virtual ViewDataType type() const;

    virtual void setShowAbsoluteValues(bool absoluteValues) = 0;

    virtual QAbstractItemModel* model() const;

    virtual void setupView(QSharedPointer<AbstractModelInstance> modelInstance) = 0;

    virtual void setSearchSelection(const gams::studio::modelinspector::SearchResult &result);

    virtual void reset() = 0;

    virtual void updateView() = 0;

    int view() const;
    virtual void setView(int view);

    QSharedPointer<AbstractViewConfiguration> viewConfig() const;
    void setViewConfig(QSharedPointer<AbstractViewConfiguration> viewConfig);

    QList<SearchResult> search(const QString &term, bool isRegEx, ViewDataType type);

    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void filtersChanged();

protected slots:
    virtual void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                          Qt::Orientation orientation) = 0;

protected:
    Ui::StandardTableViewFrame* ui;
    QSharedPointer<AbstractModelInstance> mModelInstance;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
};

}
}
}

#endif // ABSTRACTTABLEVIEWFRAME_H
