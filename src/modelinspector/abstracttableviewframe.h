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

class AbstractTableViewFrame : public QFrame
{
    Q_OBJECT

public:
    AbstractTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractTableViewFrame() = default;

    virtual AbstractTableViewFrame* clone(int view) = 0;

    virtual const IdentifierFilter& identifierFilter() const;

    virtual const IdentifierFilter& defaultIdentifierFilter() const;

    virtual void setIdentifierFilter(const IdentifierFilter &filter) = 0;

    virtual const ValueFilter& valueFilter() const;

    virtual const ValueFilter& defaultValueFilter() const;

    virtual void setValueFilter(const ValueFilter &filter);

    virtual const LabelFilter& labelFilter() const;

    virtual const LabelFilter& defaultLabelFilter() const;

    virtual void setLabelFilter(const LabelFilter &filter);

    virtual const Aggregation& aggregation() const;

    virtual const Aggregation& appliedAggregation() const = 0;

    virtual const Aggregation& defaultAggregation() const;

    virtual void setAggregation(const Aggregation &aggregation, int view) = 0;

    virtual PredefinedViewEnum type() const;

    virtual void setShowAbsoluteValues(bool absoluteValues) = 0;

    virtual QAbstractItemModel* model() const;

    virtual void setupView(QSharedPointer<AbstractModelInstance> modelInstance, int view) = 0;

    virtual void setSearchSelection(const gams::studio::modelinspector::SearchResult &result);

    virtual void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter) = 0;

    virtual void reset(PredefinedViewEnum view) = 0;

    virtual void updateView() = 0;

    QList<SearchResult> search(const QString &term, bool isRegEx);

    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void newModelView(gams::studio::modelinspector::PredefinedViewEnum type);

private slots:
    virtual void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                          Qt::Orientation orientation) = 0;

signals:
    void filtersChanged();

protected:
    IdentifierStates defaultSymbolFilter(QAbstractItemModel *model,
                                         Qt::Orientation orientation) const;

    virtual Aggregation getDefaultAggregation() const;

    virtual Aggregation appliedAggregation(const Aggregation &aggregation, int view) const;

    virtual void updateValueFilter() = 0;

protected:
    Ui::StandardTableViewFrame* ui;
    QSharedPointer<AbstractModelInstance> mModelInstance;

    LabelFilter mCurrentLabelFilter;
    LabelFilter mDefaultLabelFilter;

    IdentifierFilter mCurrentIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;

    ValueFilter mCurrentValueFilter;
    ValueFilter mDefaultValueFilter;

    Aggregation mCurrentAggregation;
    Aggregation mDefaultAggregation;
};

}
}
}

#endif // ABSTRACTTABLEVIEWFRAME_H
