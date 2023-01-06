#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QVariant>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

class Aggregation;
class AbstractModelInstance;
class AbstractViewConfiguration;

typedef QMap<Qt::Orientation, QList<int>> SectionMapping;

class DataRow
{
public:
    DataRow()
        : mEntries(0)
        , mColIdx(nullptr)
        , mData(nullptr)
    {

    }

    DataRow(int entries)
        : mEntries(entries)
    {
        mColIdx = new int[mEntries];
        mData = new double[mEntries];
    }

    DataRow(const DataRow& other)
        : DataRow(other.entries())
    {
        std::copy(other.mColIdx, other.mColIdx+other.mEntries, mColIdx);
        std::copy(other.mData, other.mData+other.mEntries, mData);
    }

    DataRow(DataRow&& other) noexcept
        : mEntries(other.mEntries)
        , mColIdx(other.mColIdx)
        , mData(other.mData)
    {
        other.mEntries = 0;
        other.mColIdx = nullptr;
        other.mData = nullptr;
    }

    ~DataRow()
    {
        if (mColIdx) delete[] mData;
        if (mData) delete[] mColIdx;
    }

    int entries() const
    {
        return mEntries;
    }

    void setEntries(int entries)
    {
        mEntries = entries;
    }

    int* colIdx() const
    {
        return mColIdx;
    }

    void setColIdx(int* colIdx)
    {
        mColIdx = colIdx;
    }

    double* data() const
    {
        return mData;
    }

    void setData(double* data)
    {
        mData = data;
    }

    auto& operator=(const DataRow& other)
    {
        delete [] mColIdx;
        delete [] mData;
        mEntries = other.mEntries;
        mColIdx = new int[mEntries];
        mData = new double[mEntries];
        std::copy(other.mColIdx, other.mColIdx+other.mEntries, mColIdx);
        std::copy(other.mData, other.mData+other.mEntries, mData);
        return *this;
    }

    auto& operator=(DataRow&& other) noexcept
    {
        mEntries = other.mEntries;
        mColIdx = other.mColIdx;
        mData = other.mData;
        other.mEntries = 0;
        other.mColIdx = nullptr;
        other.mData = nullptr;
        return *this;
    }

private:
    int mEntries;
    int* mColIdx;
    double *mData;
};

class DataMatrix
{
public:
    DataMatrix()
        : mRowCount(0)
        , mRows(nullptr)
    {

    }

    DataMatrix(int rows)
        : mRowCount(rows)
    {
        mRows = new DataRow[mRowCount];
    }

    DataMatrix(const DataMatrix& other)
        : mRowCount(other.mRowCount)
    {
        mRows = new DataRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
    }

    DataMatrix(DataMatrix&& other) noexcept
        : mRowCount(other.mRowCount)
        , mRows(other.mRows)
    {
        other.mRowCount = 0;
        other.mRows = nullptr;
    }

    ~DataMatrix()
    {
        if (mRows) delete[] mRows;
    }

    inline int rowCount() const
    {
        return mRowCount;
    }

    inline DataRow* row(int row)
    {
        return (row < 0 || row > mRowCount) ? nullptr : mRows+row;
    }

    auto& operator=(const DataMatrix& other)
    {
        delete [] mRows;
        mRowCount = other.mRowCount;
        mRows = new DataRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
        return *this;
    }

    auto& operator=(DataMatrix&& other) noexcept
    {
        mRowCount = other.mRowCount;
        mRows = other.mRows;
        other.mRowCount = 0;
        other.mRows = nullptr;
        return *this;
    }

private:
    int mRowCount;
    DataRow *mRows;
};

class DataHandler
{
public:
    class AbstractDataAggregator;

    DataHandler(AbstractModelInstance& modelInstance);

    void aggregate(QSharedPointer<AbstractViewConfiguration> viewConfig);

    QVariant data(int row, int column, int view) const;

    QVariant data(int row, int column) const;

    int headerData(int logicalIndex, Qt::Orientation orientation, int view) const;

    int rowCount(int view) const;

    int rowEntries(int row, int view) const;

    int columnCount(int view) const;

    int columnEntries(int column, int view) const;

    QSharedPointer<AbstractViewConfiguration> clone(int view, int newView);

    void loadJaccobian();

private:
    AbstractDataAggregator* cloneAggregator(int view);
    void newAggregator(QSharedPointer<AbstractViewConfiguration> viewConfig);

private:
    AbstractModelInstance& mModelInstance;
    QSharedPointer<AbstractDataAggregator> mDataAggregator;

    DataMatrix mDataMatrix;
    QMap<int, QSharedPointer<AbstractDataAggregator>> mDataCache;
};

}
}
}

#endif // DATAHANDLER_H
