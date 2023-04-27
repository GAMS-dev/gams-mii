#include "datahandler.h"
#include "abstractmodelinstance.h"
#include "aggregation.h"
#include "viewconfigurationprovider.h"

#include <algorithm>
#include <functional>
#include <vector>

#include <QSet>

#include <QDebug>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

class DataHandler::AbstractDataAggregator
{
protected:
    AbstractDataAggregator(DataHandler *dataHandler,
                           AbstractModelInstance& modelInstance,
                           QSharedPointer<AbstractViewConfiguration> viewConfig)
        : mDataHandler(dataHandler)
        , mModelInstance(modelInstance)
        , mViewConfig(viewConfig)
    {

    }

    AbstractDataAggregator(const AbstractDataAggregator& other)
        : mDataHandler(other.mDataHandler)
        , mModelInstance(other.mModelInstance)
        , mLogicalSectionMapping(other.mLogicalSectionMapping)
        , mViewConfig(other.mViewConfig->clone())
    {

    }

    AbstractDataAggregator(AbstractDataAggregator&& other) noexcept
        : mDataHandler(other.mDataHandler)
        , mModelInstance(other.mModelInstance)
        , mLogicalSectionMapping(other.mLogicalSectionMapping)
        , mViewConfig(other.mViewConfig)
    {
        mLogicalSectionMapping.clear();
        other.mViewConfig = nullptr;
    }

public:
    virtual ~AbstractDataAggregator()
    {
    }

    //const Aggregation& aggregation() const
    //{
    //    return mAppliedAggregation;
    //}

    //const AggregationMap& aggregationMap() const
    //{
    //    return mAppliedAggregation.aggregationMap();
    //}

    DataRow* dataRow(int row)
    {
        return mDataHandler->mDataMatrix.row(row);
    }

    virtual void aggregate() = 0;

    virtual double data(int row, int column) const = 0;

    virtual int headerData(Qt::Orientation orientation, int logicalIndex) const
    {
        if (logicalIndex >= 0 && logicalIndex < mLogicalSectionMapping[orientation].size()) {
            return mLogicalSectionMapping[orientation].at(logicalIndex);
        }
        return -1;
    }

    virtual int columnCount() const
    {// TODO make abstract
        return 0;
    }

    virtual int columnEntries(int column) const
    {
        Q_UNUSED(column);
        return 0;
    }

    virtual int rowCount() const
    {// TODO make abstract
        return 0;
    }

    virtual int rowEntries(int row) const
    {
        Q_UNUSED(row);
        return 0;
    }

    QSharedPointer<AbstractViewConfiguration> viewConfig() const
    {
        return mViewConfig;
    }

    //bool useAbsoluteValues() const
    //{
    //    return mAppliedAggregation.useAbsoluteValues();
    //}

    //void setFilterForTargetIndexChecked(Qt::Orientation orientation,
    //                                    int symIndex, int targetIndex)
    //{
    //    mAppliedAggregation.setLabelState(orientation,
    //                                      symIndex,
    //                                      targetIndex,
    //                                      Qt::Checked);
    //}

    auto& operator=(const AbstractDataAggregator& other)
    {
        mDataHandler = other.mDataHandler;
        mModelInstance = other.mModelInstance;
        mLogicalSectionMapping = other.mLogicalSectionMapping;
        mViewConfig = QSharedPointer<AbstractViewConfiguration>(other.mViewConfig->clone());
        return *this;
    }

    auto& operator=(AbstractDataAggregator&& other) noexcept
    {
        mDataHandler = other.mDataHandler;
        mModelInstance = other.mModelInstance;
        mLogicalSectionMapping = other.mLogicalSectionMapping;
        mLogicalSectionMapping.clear();
        mViewConfig = other.mViewConfig;
        other.mViewConfig = nullptr;
        return *this;
    }

protected:
    DataHandler *mDataHandler;
    AbstractModelInstance& mModelInstance;
    SectionMapping mLogicalSectionMapping;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
    double mDataMinimum = std::numeric_limits<double>::lowest();
    double mDataMaximum = std::numeric_limits<double>::max();
};

class IdentityDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    IdentityDataAggregator(DataHandler *dataHandler,
                           AbstractModelInstance& modelInstance)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, nullptr)
    {

    }

    virtual void aggregate() override
    {

    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class CountDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    CountDataAggregator(DataHandler *dataHandler,
                        AbstractModelInstance& modelInstance,
                        QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class MeanDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MeanDataAggregator(DataHandler *dataHandler,
                       AbstractModelInstance& modelInstance,
                       QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class MedianDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MedianDataAggregator(DataHandler *dataHandler,
                         AbstractModelInstance& modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class MinDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MinDataAggregator(DataHandler *dataHandler,
                      AbstractModelInstance& modelInstance,
                      QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class MaxDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MaxDataAggregator(DataHandler *dataHandler,
                      AbstractModelInstance& modelInstance,
                      QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class SumDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    SumDataAggregator(DataHandler *dataHandler,
                      AbstractModelInstance& modelInstance,
                      QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {

    }

    virtual void aggregate() override
    {
        // TODO !!! implement
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class MinMaxDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MinMaxDataAggregator(DataHandler *dataHandler,
                         AbstractModelInstance& modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {
        mRowCount = mModelInstance.rowCount(ViewDataType::MinMax);
        mColumnCount = mModelInstance.columnCount(ViewDataType::MinMax);
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            if (r % 2) {
                std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount, std::numeric_limits<double>::lowest());
            } else {
                std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount, std::numeric_limits<double>::max());
            }
        }
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
    }

    MinMaxDataAggregator(const MinMaxDataAggregator& other)
        : DataHandler::AbstractDataAggregator(other)
        , mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
    {
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
    }

    MinMaxDataAggregator(MinMaxDataAggregator&& other) noexcept
        : DataHandler::AbstractDataAggregator(other)
        , mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
    {
        other.mRowCount = 0;
        other.mColumnCount = 0;
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
    }

    ~MinMaxDataAggregator()
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
    }

    virtual void aggregate() override
    {
        for (int row=0; row<mRowCount; ++row) {
            mLogicalSectionMapping[Qt::Vertical].append(row);
        }
        for (const auto& variable : mModelInstance.variables()) {
            mLogicalSectionMapping[Qt::Horizontal].append(variable->firstSection());
        }
        mViewConfig->currentAggregation().useAbsoluteValues() ? aggregateAbs() : aggregateId();
    }

    double data(int row, int column) const override
    {
        return mDataMatrix[row][column];
    }

    auto& operator=(const MinMaxDataAggregator& other)
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
        mRowCount = other.mRowCount;
        mColumnCount = other.mColumnCount;
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
        return *this;
    }

    auto& operator=(MinMaxDataAggregator&& other) noexcept
    {
        mRowCount = other.mRowCount;
        other.mRowCount = 0;
        mColumnCount = other.mColumnCount;
        other.mColumnCount = 0;
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
        return *this;
    }

private:
    void aggregateId()
    {
        int minRow = 0, maxRow = 1;
        for (const auto& equation : mModelInstance.equations()) {
            for (int r=equation->firstSection(); r<=equation->lastSection(); ++r) {
                auto sparseRow = dataRow(r);
                for (int i=0; i<sparseRow->entries(); ++i) {
                    auto column = mModelInstance.variable(sparseRow->colIdx()[i])->logicalIndex();
                    mDataMatrix[minRow][column] = std::min(sparseRow->data()[i], mDataMatrix[minRow][column]);
                    mDataMatrix[maxRow][column] = std::max(sparseRow->data()[i], mDataMatrix[maxRow][column]);
                }
            }
            for (int c=0; c<mColumnCount; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            minRow += 2;
            maxRow += 2;
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        mModelInstance.setModelMinimum(mDataMinimum, ViewDataType::Jaccobian);
        mModelInstance.setModelMaximum(mDataMaximum, ViewDataType::Jaccobian);
    }

    void aggregateAbs()
    {
        int minRow = 0, maxRow = 1;
        for (const auto& equation : mModelInstance.equations()) {
            for (int r=equation->firstSection(); r<=equation->lastSection(); ++r) {
                auto sparseRow = dataRow(r);
                for (int i=0; i<sparseRow->entries(); ++i) {
                    auto column = mModelInstance.variable(sparseRow->colIdx()[i])->logicalIndex();
                    mDataMatrix[minRow][column] = std::min(std::abs(sparseRow->data()[i]), mDataMatrix[minRow][column]);
                    mDataMatrix[maxRow][column] = std::max(std::abs(sparseRow->data()[i]), mDataMatrix[maxRow][column]);
                }
            }
            for (int c=0; c<mColumnCount; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            minRow += 2;
            maxRow += 2;
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        mModelInstance.setModelMinimum(mDataMinimum, ViewDataType::Jaccobian);
        mModelInstance.setModelMaximum(mDataMaximum, ViewDataType::Jaccobian);
    }

    void setEmtpyCell(int row, int column)
    {
        if (mDataMatrix[row][column] == std::numeric_limits<double>::lowest() ||
                mDataMatrix[row][column] == std::numeric_limits<double>::max()) {
            mDataMatrix[row][column] = 0.0;
        }
    }

private:
    int mRowCount;
    int mColumnCount;
    double** mDataMatrix;
};

class SymbolsDataAggregator : public DataHandler::AbstractDataAggregator
{
private:
    class SymbolRow
    {
    public:
        SymbolRow()
        {

        }

        SymbolRow(const SymbolRow& other)
            : mEntries(other.mEntries)
            , mFirstIdx(other.mFirstIdx)
        {
            mData = new double[mEntries];
            std::copy(other.mData, other.mData+other.mEntries, mData);
        }

        SymbolRow(SymbolRow&& other) noexcept
            : mEntries(other.mEntries)
            , mFirstIdx(other.mFirstIdx)
            , mData(other.mData)
        {
            other.mEntries = 0;
            other.mFirstIdx = 0;
            other.mData = nullptr;
        }

        ~SymbolRow()
        {
            if (mData) delete [] mData;
        }

        inline int entries() const
        {
            return mEntries;
        }

        inline void setEntries(int entries)
        {
            mEntries = entries;
        }

        inline int firstIdx() const
        {
            return mFirstIdx;
        }

        inline void setFirstIdx(int firstIdx)
        {
            mFirstIdx = firstIdx;
        }

        inline int lastIdx() const
        {
            return mFirstIdx + mEntries - 1;
        }

        inline double* data()
        {
            return mData;
        }

        inline void setData(double* data)
        {
            mData = data;
        }

        auto& operator=(const SymbolRow& other)
        {
            delete [] mData;
            mEntries = other.mEntries;
            mFirstIdx = other.mFirstIdx;
            mData = new double[mEntries];
            std::copy(other.mData, other.mData+other.mEntries, mData);
            return *this;
        }

        auto& operator=(SymbolRow&& other) noexcept
        {
            mEntries = other.mEntries;
            other.mEntries = 0;
            mFirstIdx = other.mFirstIdx;
            other.mFirstIdx = 0;
            mData = other.mData;
            other.mData = nullptr;
            return *this;
        }

    private:
        int mEntries = 0;
        int mFirstIdx = 0;
        double* mData = nullptr;
    };

public:
    SymbolsDataAggregator(DataHandler *dataHandler,
                          AbstractModelInstance& modelInstance,
                          QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataAggregator(dataHandler, modelInstance, viewConfig)
    {
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
    }

    SymbolsDataAggregator(const SymbolsDataAggregator& other)
        : DataHandler::AbstractDataAggregator(other)
        , mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
    {
        mRows = new SymbolRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
        mColumnEntryCount = new int[mColumnCount];
        std::copy(other.mColumnEntryCount, other.mColumnEntryCount+other.mColumnCount, mColumnEntryCount);
    }

    SymbolsDataAggregator(SymbolsDataAggregator&& other) noexcept
        : DataHandler::AbstractDataAggregator(other)
        , mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
        , mRows(other.mRows)
        , mColumnEntryCount(other.mColumnEntryCount)
    {
        other.mRowCount = 0;
        other.mColumnCount = 0;
        other.mRows = nullptr;
        other.mColumnEntryCount = nullptr;
    }

    ~SymbolsDataAggregator()
    {
        if (mColumnEntryCount) delete [] mColumnEntryCount;
        if (mRows) delete [] mRows;
    }

    void aggregate() override
    {
        Symbol* equation = nullptr;
        for (const auto &filter : std::as_const(mViewConfig->currentIdentifierFilter()[Qt::Vertical])) {
            if (filter.Checked == Qt::Unchecked)
                continue;
            equation = mModelInstance.equation(filter.SymbolIndex);
            break;
        }
        if (!equation) return;
        mRowCount = equation->entries();
        mRows = new SymbolRow[mRowCount];
        Symbol* variable = nullptr;
        for (const auto &filter : std::as_const(mViewConfig->currentIdentifierFilter()[Qt::Horizontal])) {
            if (filter.Checked == Qt::Unchecked)
                continue;
            variable = mModelInstance.variable(filter.SymbolIndex);
            for (int s=variable->firstSection(); s<=variable->lastSection(); ++s) {
                mLogicalSectionMapping[Qt::Horizontal].append(s);
            }
            mColumnCount = mLogicalSectionMapping[Qt::Horizontal].size();
            break;
        }
        if (!variable) return;
        mColumnEntryCount = new int[mColumnCount];
        std::fill(mColumnEntryCount, mColumnEntryCount+mColumnCount, 0);
        mViewConfig->currentValueFilter().UseAbsoluteValues ? aggregateAbs(equation, variable)
                                                            : aggregateId(equation, variable);
    }

    double data(int row, int column) const override
    {
        if (!mRows || !mRows[row].entries())
            return 0.0;
        if (column < mRows[row].firstIdx() || column > mRows[row].lastIdx())
            return 0.0;
        return mRows[row].data()[column-mRows[row].firstIdx()];
    }

    int columnCount() const override
    {
        return mLogicalSectionMapping[Qt::Horizontal].size();
    }

    int columnEntries(int column) const override
    {
        return column < mColumnCount ? mColumnEntryCount[column] : 0;
    }

    int rowCount() const override
    {
        return mLogicalSectionMapping[Qt::Vertical].size();
    }

    int rowEntries(int row) const override
    {
        return row < mRowCount ? mRows[row].entries() : 0;
    }

    auto& operator=(const SymbolsDataAggregator& other)
    {
        delete [] mRows;
        delete [] mColumnEntryCount;
        mRowCount = other.mRowCount;
        mColumnCount = other.mColumnCount;
        mRows = new SymbolRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
        mColumnEntryCount = new int[mColumnCount];
        std::copy(other.mColumnEntryCount, other.mColumnEntryCount+other.mColumnCount, mColumnEntryCount);
        return *this;
    }

    auto& operator=(SymbolsDataAggregator&& other) noexcept
    {
        mRowCount = other.mRowCount;
        other.mRowCount = 0;
        mColumnCount = other.mColumnCount;
        other.mColumnCount = 0;
        mRows = other.mRows;
        other.mRows = nullptr;
        mColumnEntryCount = other.mColumnEntryCount;
        other.mColumnEntryCount = nullptr;
        return *this;
    }

private:
    void aggregateAbs(Symbol *equation, Symbol* variable)
    {
        mColumnCount = variable->entries();
        for (int r=equation->firstSection(), rr=0; r<=equation->lastSection(); ++r, ++rr) {
            mLogicalSectionMapping[Qt::Vertical].append(r);
            auto sparseRow = dataRow(r);
            int sym_nz = 0, start_i = -1, start_c = 0, end_c = 0;
            for (int i=0; i<sparseRow->entries(); ++i) {
                if (sparseRow->colIdx()[i] > variable->lastSection()) {
                    break;
                }
                if (sparseRow->colIdx()[i] < variable->firstSection()) {
                    continue;
                }
                if (start_i < 0) {
                    start_c = sparseRow->colIdx()[i];
                    start_i = i;
                }
                end_c = sparseRow->colIdx()[i];
                ++sym_nz;
            }
            if (!sym_nz) continue;
            auto* row = &mRows[rr];
            row->setEntries(end_c + 1 - start_c);
            row->setData(new double[row->entries()]);
            row->setFirstIdx(start_c - variable->firstSection());
            if (sym_nz == row->entries()) {
                for (int nz=start_i, c=0; nz<start_i+sym_nz; ++nz, ++c) {
                    row->data()[c] = std::abs(sparseRow->data()[nz]);
                    mDataMinimum = std::min(mDataMinimum, row->data()[c]);
                    mDataMaximum = std::max(mDataMaximum, row->data()[c]);
                    ++mColumnEntryCount[row->firstIdx()+c];
                }
            } else {
                std::fill(row->data(), row->data()+row->entries(), 0.0);
                for (int nz=start_i, c=0; nz<start_i+sym_nz; ++nz) {
                    c = sparseRow->colIdx()[nz] - start_c;
                    row->data()[c] = std::abs(sparseRow->data()[nz]);
                    mDataMinimum = std::min(mDataMinimum, row->data()[c]);
                    mDataMaximum = std::max(mDataMaximum, row->data()[c]);
                    ++mColumnEntryCount[row->firstIdx()+c];
                }
            }
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
    }

    void aggregateId(Symbol *equation, Symbol* variable)
    {
        mColumnCount = variable->entries();
        for (int r=equation->firstSection(), rr=0; r<=equation->lastSection(); ++r, ++rr) {
            mLogicalSectionMapping[Qt::Vertical].append(r);
            auto sparseRow = dataRow(r);
            int sym_nz = 0, start_i = -1, start_c = 0, end_c = 0;
            for (int i=0; i<sparseRow->entries(); ++i) {
                if (sparseRow->colIdx()[i] > variable->lastSection()) {
                    break;
                }
                if (sparseRow->colIdx()[i] < variable->firstSection()) {
                    continue;
                }
                if (start_i < 0) {
                    start_c = sparseRow->colIdx()[i];
                    start_i = i;
                }
                end_c = sparseRow->colIdx()[i];
                ++sym_nz;
            }
            if (!sym_nz) continue;
            auto* row = &mRows[rr];
            row->setEntries(end_c + 1 - start_c);
            row->setData(new double[row->entries()]);
            row->setFirstIdx(start_c - variable->firstSection());
            if (sym_nz == row->entries()) {
                for (int nz=start_i, c=0; nz<start_i+sym_nz; ++nz, ++c) {
                    row->data()[c] = sparseRow->data()[nz];
                    mDataMinimum = std::min(mDataMinimum, row->data()[c]);
                    mDataMaximum = std::max(mDataMaximum, row->data()[c]);
                    ++mColumnEntryCount[row->firstIdx()+c];
                }
            } else {
                std::fill(row->data(), row->data()+row->entries(), 0.0);
                for (int nz=start_i, c=0; nz<start_i+sym_nz; ++nz) {
                    c = sparseRow->colIdx()[nz] - start_c;
                    row->data()[c] = sparseRow->data()[nz];
                    mDataMinimum = std::min(mDataMinimum, row->data()[c]);
                    mDataMaximum = std::max(mDataMaximum, row->data()[c]);
                    ++mColumnEntryCount[row->firstIdx()+c];
                }
            }
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
    }

private:
    int mRowCount = 0;
    int mColumnCount = 0;
    SymbolRow* mRows = nullptr;
    int* mColumnEntryCount = nullptr;
};

DataHandler::DataHandler(AbstractModelInstance& modelInstance)
    : mModelInstance(modelInstance)
    , mDataAggregator(QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this, mModelInstance)))
{
}

void DataHandler::aggregate(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    if (!viewConfig) return;
    newAggregator(viewConfig);
    mDataCache.remove(viewConfig->view());
    if (viewConfig->currentAggregation().type() != Aggregation::None) {
        mDataAggregator->aggregate();
    } else {
        return;
    }
    mDataCache[viewConfig->view()] = mDataAggregator;
}

QVariant DataHandler::data(int row, int column, int view) const
{
    if (mDataCache.contains(view) && mDataCache[view]->data(row, column) != 0.0) {
        return mDataCache[view]->data(row, column);
    }
    return QVariant();
}

QVariant DataHandler::data(int row, int column) const
{// TODO (AF) implement or remove
    Q_UNUSED(row);
    Q_UNUSED(column);
    return QVariant(); //mDataMatrix.value(row, column);
}

int DataHandler::headerData(int logicalIndex,
                            Qt::Orientation orientation,
                            int view) const
{
    return mDataCache.contains(view) ? mDataCache[view]->headerData(orientation, logicalIndex) : -1;
}

int DataHandler::rowCount(int view) const
{
    if (mDataCache.contains(view)) {
        return mDataCache[view]->rowCount();
    }
    return 0;
}

int DataHandler::rowEntries(int row, int view) const
{
    return mDataCache.contains(view) ? mDataCache[view]->rowEntries(row) : 0;
}

int DataHandler::columnCount(int view) const
{
    if (mDataCache.contains(view)) {
        return mDataCache[view]->columnCount();
    }
    return 0;
}

int DataHandler::columnEntries(int column, int view) const
{
    return mDataCache.contains(view) ? mDataCache[view]->columnEntries(column) : 0;
}

QSharedPointer<AbstractViewConfiguration> DataHandler::clone(int view, int newView)
{
    if (!mDataCache.contains(view))
        return nullptr;
    mDataCache[newView] = QSharedPointer<AbstractDataAggregator>(cloneAggregator(view));
    mDataCache[newView]->viewConfig()->setView(newView);
    return mDataCache[newView]->viewConfig();
}

void DataHandler::loadJaccobian()
{
    mDataMatrix = DataMatrix(mModelInstance.equationRowCount());
    mModelInstance.jaccobianData(mDataMatrix);
}

DataHandler::AbstractDataAggregator* DataHandler::cloneAggregator(int view)
{
    switch(mDataCache[view]->viewConfig()->currentAggregation().type()) {
    case Aggregation::Count:
    {
        auto aggregator = static_cast<CountDataAggregator*>(mDataCache[view].get());
        return new CountDataAggregator(*aggregator);
    }
    case Aggregation::Mean:
    {
        auto aggregator = static_cast<MeanDataAggregator*>(mDataCache[view].get());
        return new MeanDataAggregator(*aggregator);
    }
    case Aggregation::Median:
    {
        auto aggregator = static_cast<MedianDataAggregator*>(mDataCache[view].get());
        return new MedianDataAggregator(*aggregator);
    }
    case Aggregation::Maximum:
    {
        auto aggregator = static_cast<MaxDataAggregator*>(mDataCache[view].get());
        return new MaxDataAggregator(*aggregator);
    }
    case Aggregation::Minimum:
    {
        auto aggregator = static_cast<MinDataAggregator*>(mDataCache[view].get());
        return new MinDataAggregator(*aggregator);
    }
    case Aggregation::Sum:
    {
        auto aggregator = static_cast<SumDataAggregator*>(mDataCache[view].get());
        return new SumDataAggregator(*aggregator);
    }
    case Aggregation::MinMax:
    {
        auto aggregator = static_cast<MinMaxDataAggregator*>(mDataCache[view].get());
        return new MinMaxDataAggregator(*aggregator);
    }
    case Aggregation::Symols:
    {
        auto aggregator = static_cast<SymbolsDataAggregator*>(mDataCache[view].get());
        return new SymbolsDataAggregator(*aggregator);
    }
    default:
    {
        return new IdentityDataAggregator(this, mModelInstance);
    }
    }
}

void DataHandler::newAggregator(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    switch (viewConfig->currentAggregation().type()) {
    case Aggregation::Count:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new CountDataAggregator(this,
                                                                                         mModelInstance,
                                                                                         viewConfig));
        break;
    case Aggregation::Mean:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MeanDataAggregator(this,
                                                                                        mModelInstance,
                                                                                        viewConfig));
        break;
    case Aggregation::Median:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MedianDataAggregator(this,
                                                                                          mModelInstance,
                                                                                          viewConfig));
        break;
    case Aggregation::Maximum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MaxDataAggregator(this,
                                                                                       mModelInstance,
                                                                                       viewConfig));
        break;
    case Aggregation::Minimum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MinDataAggregator(this,
                                                                                       mModelInstance,
                                                                                       viewConfig));
        break;
    case Aggregation::Sum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new SumDataAggregator(this,
                                                                                       mModelInstance,
                                                                                       viewConfig));
        break;
    case Aggregation::MinMax:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MinMaxDataAggregator(this,
                                                                                          mModelInstance,
                                                                                          viewConfig));
        break;
    case Aggregation::Symols:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new SymbolsDataAggregator(this,
                                                                                           mModelInstance,
                                                                                           viewConfig));
        break;
    default:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this, mModelInstance));
        break;
    }
}

}
}
}
