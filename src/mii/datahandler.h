/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QVariant>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace mii {

class Aggregation;
class AbstractModelInstance;
class AbstractViewConfiguration;
class PostoptTreeItem;

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

    QVariant value(int index, int lastSymIndex)
    {
        int entries = lastSymIndex < mEntries ? lastSymIndex+1 : mEntries;
        for (int i=0; i<entries; ++i) {
            if (mColIdx[i] == index) {
                return mData[i];
            }
        }
        return QVariant();
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
    class AbstractDataProvider;

    struct CoefficientCount
    {
        CoefficientCount()
        {

        }

        CoefficientCount(const CoefficientCount& other)
            : mColumnCount(other.mColumnCount)
            , mRowCount(other.mRowCount)
        {
            mCount = new int*[mRowCount];
            for (int r=0; r<mRowCount; ++r) {
                mCount[r] = new int[mColumnCount];
                std::copy(other.mCount[r], other.mCount[r]+other.mColumnCount, mCount[r]);
            }
        }

        CoefficientCount(CoefficientCount&& other) noexcept
            : mCount(other.mCount)
            , mColumnCount(other.mColumnCount)
            , mRowCount(other.mRowCount)
        {
            other.mCount = nullptr;
            other.mColumnCount = 0;
            other.mRowCount = 0;
        }

        CoefficientCount(int columnCount, int rowCount)
            : mColumnCount(columnCount)
            , mRowCount(rowCount)
        {
            mCount = new int*[mRowCount];
            for (int r=0; r<mRowCount; ++r) {
                mCount[r] = new int[mColumnCount];
                std::fill(mCount[r], mCount[r]+mColumnCount, 0);
            }
        }

        ~CoefficientCount()
        {
            for (int r=0; r<mRowCount; ++r) {
                delete [] mCount[r];
            }
            if (mCount) delete [] mCount;
        }

        int** count()
        {
            return mCount;
        }

        int columnCount() const
        {
            return mColumnCount;
        }

        int rowCount() const
        {
            return mRowCount;
        }

        auto& operator=(const CoefficientCount& other)
        {
            mRowCount = other.mColumnCount;
            mColumnCount = other.mColumnCount;
            for (int r=0; r<mRowCount; ++r) {
                delete [] mCount[r];
            }
            delete [] mCount;
            mCount = new int*[mRowCount];
            for (int r=0; r<mRowCount; ++r) {
                mCount[r] = new int[mColumnCount];
                std::copy(other.mCount[r], other.mCount[r]+other.mColumnCount, mCount[r]);
            }
            return *this;
        }

        auto& operator=(CoefficientCount&& other) noexcept
        {
            mCount = other.mCount;
            other.mCount = nullptr;
            mColumnCount = other.mColumnCount;
            other.mColumnCount = 0;
            mRowCount = other.mRowCount;
            other.mRowCount = 0;
            return *this;
        }

    private:
        int** mCount = nullptr;
        int mColumnCount = 0;
        int mRowCount = 0;
    };

    DataHandler(AbstractModelInstance& modelInstance);

    ~DataHandler();

    void aggregate(const QSharedPointer<AbstractViewConfiguration> &viewConfig);

    void loadData(const QSharedPointer<AbstractViewConfiguration> &viewConfig);

    QVariant data(int row, int column, int viewId) const;

    QSharedPointer<PostoptTreeItem> dataTree(int viewId) const;

    void remove(int viewId);

    int headerData(int logicalIndex, Qt::Orientation orientation, int viewId) const;

    QVariant plainHeaderData(Qt::Orientation orientation,
                             int viewId, int logicalIndex,
                             int dimension) const;

    QVariant sectionLabels(Qt::Orientation orientation,
                           int viewId, int logicalIndex) const;

    int rowCount(int viewId) const;

    int rowEntries(int row, int viewId) const;

    int columnCount(int viewId) const;

    int columnEntries(int column, int viewId) const;

    int symbolRowCount(int viewId) const;

    int symbolColumnCount(int viewId) const;

    double modelMinimum() const;
    void setModelMinimum(double minimum);

    double modelMaximum() const;
    void setModelMaximum(double maximum);

    int maxSymbolDimension(int viewId, Qt::Orientation orientation);

    QSharedPointer<AbstractViewConfiguration> clone(int viewId, int newView);
    
    void loadJacobian();

private:
    AbstractDataProvider *cloneProvider(int viewId);
    QSharedPointer<AbstractDataProvider> newProvider(const QSharedPointer<AbstractViewConfiguration> &viewConfig);

private:
    AbstractModelInstance& mModelInstance;
    double mModelMinimum = std::numeric_limits<double>::max();
    double mModelMaximum = std::numeric_limits<double>::lowest();

    DataMatrix mDataMatrix;
    CoefficientCount *mCoeffCount = nullptr;

    ///
    /// \brief Abstract data provider cache, where key is the view ID.
    ///
    QMap<int, QSharedPointer<AbstractDataProvider>> mDataCache;
};

}
}
}

#endif // DATAHANDLER_H
