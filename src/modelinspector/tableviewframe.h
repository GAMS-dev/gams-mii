#ifndef TABLEVIEWFRAME_H
#define TABLEVIEWFRAME_H

#include <QFrame>
#include <QSharedPointer>

#include "common.h"
#include "aggregation.h"

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class TableViewFrame;
}

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
class ModelInstance;

class TableViewFrame : public QFrame
{
    Q_OBJECT

public:
    TableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    virtual TableViewFrame* clone(int view) = 0;

    const IdentifierFilter& identifierFilter() const
    {
        return mCurrentIdentifierFilter;
    }

    const IdentifierFilter& defaultIdentifierFilter() const
    {
        return mDefaultIdentifierFilter;
    }

    virtual void setIdentifierFilter(const IdentifierFilter &filter)
    {
        mCurrentIdentifierFilter = filter;
    }

    const ValueFilter& valueFilter() const
    {
        return mCurrentValueFilter;
    }

    const ValueFilter& defaultValueFilter() const
    {
        return mDefaultValueFilter;
    }

    virtual void setValueFilter(const ValueFilter &filter)
    {
        mCurrentValueFilter = filter;
    }

    const LabelFilter& labelFilter() const
    {
        return mCurrentLabelFilter;
    }

    const LabelFilter& defaultLabelFilter() const
    {
        return mDefaultLabelFilter;
    }

    virtual void setLabelFilter(const LabelFilter &filter)
    {
        mCurrentLabelFilter = filter;
    }

    const Aggregation& aggregation() const
    {
        return mCurrentAggregation;
    }

    const Aggregation& defaultAggregation() const
    {
        return mDefaultAggregation;
    }

    virtual void setAggregation(const Aggregation &aggregation, int view);

    virtual PredefinedViewEnum type() const
    {
        return PredefinedViewEnum::Unknown;
    }

    virtual DataSource horizontalDataSource() const
    {
        return DataSource::VariableData;
    }

    virtual DataSource verticalDataSource() const
    {
        return DataSource::EquationData;
    }

    virtual QAbstractItemModel* model() const = 0;

    virtual void setupView(QSharedPointer<ModelInstance> modelInstance, int view) = 0;

    virtual QList<SearchResult> searchHeaders(const QString &term, bool isRegEx) = 0;

    virtual void setSearchSelection(const gams::studio::modelinspector::SearchResult &result);

    virtual void resetColumnRowFilter() = 0;

    virtual void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter);

private slots:
    virtual void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                          Qt::Orientation orientation) = 0;

signals:
    void filtersChanged();

protected:
    IdentifierStates defaultSymbolFilter(QAbstractItemModel *model,
                                         Qt::Orientation orientation) const;


    void setIdentifierFilterCheckState(int symbolIndex, Qt::CheckState state,
                                       Qt::Orientation orientation,
                                       DataSource dataSource);

    virtual Aggregation getDefaultAggregation() const;

    Aggregation appliedAggregation(const Aggregation &aggregations, int view) const;

    void cloneFilterAndAggregation(TableViewFrame *clone, int newView);

protected:
    Ui::TableViewFrame* ui;
    QSharedPointer<ModelInstance> mModelInstance;

    LabelFilter mCurrentLabelFilter;
    LabelFilter mDefaultLabelFilter;

    IdentifierFilter mCurrentIdentifierFilter;
    IdentifierFilter mDefaultIdentifierFilter;

    ValueFilter mCurrentValueFilter;
    ValueFilter mDefaultValueFilter;

    Aggregation mCurrentAggregation;
    Aggregation mDefaultAggregation;

    HierarchicalHeaderView* mHorizontalHeader = nullptr;
    HierarchicalHeaderView* mVerticalHeader = nullptr;
    ValueFormatProxyModel* mValueFormatModel = nullptr;
    IdentifierFilterModel* mIdentifierFilterModel = nullptr;
    IdentifierLabelFilterModel* mIdentifierLabelFilterModel = nullptr;
    LabelFilterModel* mLabelFilterModel = nullptr;
    AggregationProxyModel* mAggregationModel = nullptr;
    ColumnRowFilterModel* mColumnRowFilterModel = nullptr;
};

class EqnTableViewFrame : public TableViewFrame
{
    Q_OBJECT

public:
    EqnTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    TableViewFrame* clone(int view) override;

    QAbstractItemModel* model() const override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::EqnAttributes;
    }

    void setupView(QSharedPointer<ModelInstance> modelInstance, int view) override;

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void resetColumnRowFilter() override;

protected:
    Aggregation getDefaultAggregation() const override;

private slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void searchHeader(const QString &term, bool isRegEx,
                      DataSource dataSource,
                      Qt::Orientation orientation,
                      QList<SearchResult> &result);

private:
    QSharedPointer<EquationAttributeTableModel> mBaseModel;
};

class VarTableViewFrame : public TableViewFrame
{
    Q_OBJECT

public:
    VarTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    TableViewFrame* clone(int view) override;

    QAbstractItemModel* model() const override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::VarAttributes;
    }

    DataSource horizontalDataSource() const override
    {
        return DataSource::VariableData;
    }

    DataSource verticalDataSource() const override
    {
        return DataSource::EquationData;
    }

    void setupView(QSharedPointer<ModelInstance> modelInstance, int view) override;

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void resetColumnRowFilter() override;

    void setupFiltersAggregation(QAbstractItemModel *model, const LabelFilter &filter) override;

protected:
    Aggregation getDefaultAggregation() const override;

private slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void searchHeader(const QString &term, bool isRegEx,
                      DataSource dataSource,
                      Qt::Orientation orientation,
                      QList<SearchResult> &result);

private:
    QSharedPointer<VariableAttributeTableModel> mBaseModel;
};

class JaccTableViewFrame : public TableViewFrame
{
    Q_OBJECT

public:
    JaccTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    TableViewFrame* clone(int view) override;

    QAbstractItemModel* model() const override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::Jaccobian;
    }

    void setupView(QSharedPointer<ModelInstance> modelInstance, int view) override;

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void resetColumnRowFilter() override;

public slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void searchHeader(const QString &term, bool isRegEx,
                      DataSource dataSource,
                      Qt::Orientation orientation,
                      QList<SearchResult> &result);

private:
    QSharedPointer<JaccobianTableModel> mBaseModel;
};

class FullTableViewFrame : public TableViewFrame
{
    Q_OBJECT

public:
    FullTableViewFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    TableViewFrame* clone(int view) override;

    QAbstractItemModel* model() const override;

    PredefinedViewEnum type() const override
    {
        return PredefinedViewEnum::Full;
    }

    void setupView(QSharedPointer<ModelInstance> modelInstance, int view) override;

    QList<SearchResult> searchHeaders(const QString &term, bool isRegEx) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setLabelFilter(const LabelFilter &filter) override;

    void setValueFilter(const ValueFilter &filter) override;

    void resetColumnRowFilter() override;

public slots:
    void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                  Qt::Orientation orientation) override;

private:
    void searchHeader(const QString &term, bool isRegEx,
                      DataSource dataSource,
                      Qt::Orientation orientation,
                      QList<SearchResult> &result);

private:
    QSharedPointer<ModelInstanceTableModel> mModelInstanceModel;
};

}
}
}

#endif // TABLEVIEWFRAME_H
