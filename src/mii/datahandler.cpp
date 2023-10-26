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
#include "datahandler.h"
#include "abstractmodelinstance.h"
#include "aggregation.h"
#include "viewconfigurationprovider.h"
#include "postopttreeitem.h"

#include <algorithm>
#include <functional>
#include <vector>

#include <QSet>

#include <QDebug>

using namespace std;

namespace gams {
namespace studio {
namespace mii {

class DataHandler::AbstractDataProvider
{
protected:
    AbstractDataProvider(DataHandler *dataHandler,
                         AbstractModelInstance& modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig)
        : mDataHandler(dataHandler)
        , mModelInstance(modelInstance)
        , mViewConfig(viewConfig)
    {

    }

    AbstractDataProvider(const AbstractDataProvider& other)
        : mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
        , mSymbolRowCount(other.mSymbolRowCount)
        , mSymbolColumnCount(other.mSymbolColumnCount)
        , mDataHandler(other.mDataHandler)
        , mModelInstance(other.mModelInstance)
        , mLogicalSectionMapping(other.mLogicalSectionMapping)
        , mViewConfig(other.mViewConfig->clone())
    {

    }

    AbstractDataProvider(AbstractDataProvider&& other) noexcept
        : mRowCount(other.mRowCount)
        , mColumnCount(other.mColumnCount)
        , mSymbolRowCount(other.mSymbolRowCount)
        , mSymbolColumnCount(other.mSymbolColumnCount)
        , mDataHandler(other.mDataHandler)
        , mModelInstance(other.mModelInstance)
        , mLogicalSectionMapping(other.mLogicalSectionMapping)
        , mViewConfig(other.mViewConfig)
    {
        mLogicalSectionMapping.clear();
        other.mViewConfig = nullptr;
        other.mRowCount = 0;
        other.mColumnCount = 0;
        other.mSymbolRowCount = 0;
        other.mSymbolColumnCount = 0;
    }

public:
    virtual ~AbstractDataProvider()
    {
    }

    DataRow* dataRow(int row)
    {
        return mDataHandler->mDataMatrix.row(row);
    }

    virtual void loadData() = 0;

    virtual double data(int row, int column) const = 0;

    virtual QVariant plainHeaderData(Qt::Orientation orientation,
                                     int logicalIndex,
                                     int dimension) const
    {
        const auto& labels = mViewConfig->sectionLabels(orientation);
        if (logicalIndex < labels.size()) {
            return labels[logicalIndex][dimension];
        }
        return QVariant();
    }

    virtual int headerData(Qt::Orientation orientation, int logicalIndex) const
    {
        if (logicalIndex >= 0 && logicalIndex < mLogicalSectionMapping[orientation].size()) {
            return mLogicalSectionMapping[orientation].at(logicalIndex);
        }
        return -1;
    }

    QVariant sectionLabels(Qt::Orientation orientation, int logicalIndex) const
    {
        const auto& labels = mViewConfig->sectionLabels(orientation);
        if (logicalIndex < labels.size()) {
            return labels[logicalIndex];
        }
        return QStringList();
    }

    int columnCount() const
    {
        return mColumnCount;
    }

    int rowCount() const
    {
        return mRowCount;
    }

    int symbolRowCount() const
    {
        return mSymbolRowCount;
    }

    int symbolColumnCount() const
    {
        return mSymbolColumnCount;
    }

    virtual int columnEntries(int column) const
    {
        Q_UNUSED(column);
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

    auto& operator=(const AbstractDataProvider& other)
    {
        mRowCount = other.mRowCount;
        mColumnCount = other.mColumnCount;
        mSymbolRowCount = other.mSymbolRowCount;
        mSymbolColumnCount = other.mSymbolColumnCount;
        mDataHandler = other.mDataHandler;
        mModelInstance = other.mModelInstance;
        mLogicalSectionMapping = other.mLogicalSectionMapping;
        mViewConfig = QSharedPointer<AbstractViewConfiguration>(other.mViewConfig->clone());
        return *this;
    }

    auto& operator=(AbstractDataProvider&& other) noexcept
    {
        mRowCount = other.mRowCount;
        mSymbolRowCount = other.mSymbolRowCount;
        other.mRowCount = 0;
        other.mSymbolRowCount = 0;
        mColumnCount = other.mColumnCount;
        mSymbolColumnCount = other.mSymbolColumnCount;
        other.mColumnCount = 0;
        other.mSymbolColumnCount = 0;
        mDataHandler = other.mDataHandler;
        mModelInstance = other.mModelInstance;
        mLogicalSectionMapping = other.mLogicalSectionMapping;
        mLogicalSectionMapping.clear();
        mViewConfig = other.mViewConfig;
        other.mViewConfig = nullptr;
        return *this;
    }

protected:
    int mRowCount = 0;
    int mColumnCount = 0;
    int mSymbolRowCount = 0;
    int mSymbolColumnCount = 0;
    DataHandler *mDataHandler;
    AbstractModelInstance& mModelInstance;
    SectionMapping mLogicalSectionMapping;
    QSharedPointer<AbstractViewConfiguration> mViewConfig;
    double mDataMinimum = std::numeric_limits<double>::lowest();
    double mDataMaximum = std::numeric_limits<double>::max();
};

class IdentityDataProvider : public DataHandler::AbstractDataProvider
{
public:
    IdentityDataProvider(DataHandler *dataHandler,
                         AbstractModelInstance& modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
    {
        mRowCount = mModelInstance.equationRowCount();
        mSymbolRowCount = mRowCount;
        mColumnCount = mModelInstance.variableRowCount();
        mSymbolColumnCount = mColumnCount;
    }

    virtual void loadData() override
    {

    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }
};

class BPScalingProvider final : public DataHandler::AbstractDataProvider
{
public:
    BPScalingProvider(DataHandler *dataHandler,
                      AbstractModelInstance& modelInstance,
                      QSharedPointer<AbstractViewConfiguration> viewConfig,
                      DataHandler::CoefficientCount& negPosCount)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
        , mCoeffCount(negPosCount)
    {
        mSymbolRowCount = mModelInstance.equationCount() * 2;
        mRowCount = mSymbolRowCount + 2; // one row for max and min
        mSymbolColumnCount = mModelInstance.variableCount();
        mColumnCount = mSymbolColumnCount + 2;
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            if (r % 2) {
                std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount,
                          std::numeric_limits<double>::max());
            } else {
                std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount,
                          std::numeric_limits<double>::lowest());
            }
        }
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
    }

    BPScalingProvider(const BPScalingProvider& other)
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r],
                      other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
    }

    BPScalingProvider(BPScalingProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
    }

    ~BPScalingProvider()
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
    }

    virtual void loadData() override
    {
        for (const auto& equation : mModelInstance.equations()) {
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
        }
        for (const auto& variable : mModelInstance.variables()) {
            mLogicalSectionMapping[Qt::Horizontal].append(variable->firstSection());
        }
        mViewConfig->currentValueFilter().isAbsolute() ? aggregateAbs() : aggregateId();
    }

    double data(int row, int column) const override
    {
        return mDataMatrix[row][column];
    }

    auto& operator=(const BPScalingProvider& other)
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

    auto& operator=(BPScalingProvider&& other) noexcept
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

private:
    void aggregateId()
    {
        int minRow = 1, maxRow = 0;
        for (const auto& equation : mModelInstance.equations()) {
            double rhsMin = std::numeric_limits<double>::max();
            double rhsMax = std::numeric_limits<double>::lowest();
            double eqnMin = std::numeric_limits<double>::max();
            double eqnMax = std::numeric_limits<double>::lowest();
            mCoeffCount.count()[maxRow][mColumnCount-2] = mModelInstance.equationType(equation->firstSection());
            for (int r=equation->firstSection(); r<=equation->lastSection(); ++r) {
                auto sparseRow = dataRow(r);
                auto rhs = mModelInstance.rhs(r);
                if (rhs) {
                    rhsMin = std::min(rhsMin, rhs);
                    rhsMax = std::max(rhsMax, rhs);
                    if (rhs < 0) --mCoeffCount.count()[minRow][mColumnCount-1];
                    else if (rhs > 0) ++mCoeffCount.count()[maxRow][mColumnCount-1];
                }
                for (int i=0; i<sparseRow->entries(); ++i) {
                    auto value = sparseRow->data()[i];
                    auto column = mModelInstance.variable(sparseRow->colIdx()[i])->logicalIndex();
                    mDataMatrix[minRow][column] = std::min(value, mDataMatrix[minRow][column]);
                    mDataMatrix[maxRow][column] = std::max(value, mDataMatrix[maxRow][column]);
                    if (value < 0) {
                        --mCoeffCount.count()[minRow][column];
                    } else if (value > 0) {
                        ++mCoeffCount.count()[maxRow][column];
                    }
                }
            }
            mDataMatrix[minRow][mColumnCount-2] = rhsMin;
            mDataMatrix[maxRow][mColumnCount-2] = rhsMax;
            mDataMatrix[mRowCount-1][mColumnCount-2] = std::min(mDataMatrix[mRowCount-1][mColumnCount-2], rhsMin);
            mDataMatrix[mRowCount-2][mColumnCount-2] = std::max(mDataMatrix[mRowCount-2][mColumnCount-2], rhsMax);
            for (int c=0; c<mColumnCount-2; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                mDataMatrix[mRowCount-1][c] = std::min(mDataMatrix[mRowCount-1][c], mDataMatrix[minRow][c]);
                mDataMatrix[mRowCount-2][c] = std::max(mDataMatrix[mRowCount-2][c], mDataMatrix[maxRow][c]);
                eqnMin = std::min(eqnMin, mDataMatrix[minRow][c]);
                eqnMax = std::max(eqnMax, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            mDataMatrix[minRow][mColumnCount-1] = eqnMin;
            mDataMatrix[maxRow][mColumnCount-1] = eqnMax;
            for (int c=mColumnCount-2; c<mColumnCount; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            minRow += 2;
            maxRow += 2;
        }
        setEmtpyCell(mRowCount-2, mColumnCount-2);
        setEmtpyCell(mRowCount-1, mColumnCount-2);
        mDataMatrix[mRowCount-2][mColumnCount-1] = 0.0;
        mDataMatrix[mRowCount-1][mColumnCount-1] = 0.0;
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        mDataHandler->setModelMinimum(mDataMinimum);
        mDataHandler->setModelMaximum(mDataMaximum);
    }

    void aggregateAbs()
    {
        int minRow = 1, maxRow = 0;
        for (const auto& equation : mModelInstance.equations()) {
            double rhsMin = std::numeric_limits<double>::max();
            double rhsMax = std::numeric_limits<double>::lowest();
            double eqnMin = std::numeric_limits<double>::max();
            double eqnMax = std::numeric_limits<double>::lowest();
            mCoeffCount.count()[maxRow][mColumnCount-2] = mModelInstance.equationType(equation->firstSection());
            for (int r=equation->firstSection(); r<=equation->lastSection(); ++r) {
                auto sparseRow = dataRow(r);
                auto rhs = mModelInstance.rhs(r);
                if (rhs) {
                    rhsMin = std::min(rhsMin, std::abs(rhs));
                    rhsMax = std::max(rhsMax, std::abs(rhs));
                    if (rhs < 0) ++mCoeffCount.count()[minRow][mColumnCount-1];
                    else if (rhs > 0) ++mCoeffCount.count()[maxRow][mColumnCount-1];
                }
                for (int i=0; i<sparseRow->entries(); ++i) {
                    auto value = sparseRow->data()[i];
                    auto column = mModelInstance.variable(sparseRow->colIdx()[i])->logicalIndex();
                    mDataMatrix[minRow][column] = std::min(std::abs(sparseRow->data()[i]), mDataMatrix[minRow][column]);
                    mDataMatrix[maxRow][column] = std::max(std::abs(sparseRow->data()[i]), mDataMatrix[maxRow][column]);
                    if (value < 0) {
                        ++mCoeffCount.count()[minRow][column];
                    } else if (value > 0) {
                        ++mCoeffCount.count()[maxRow][column];
                    }
                }
            }
            mDataMatrix[minRow][mColumnCount-2] = rhsMin;
            mDataMatrix[maxRow][mColumnCount-2] = rhsMax;
            mDataMatrix[mRowCount-1][mColumnCount-2] = std::min(mDataMatrix[mRowCount-1][mColumnCount-2], rhsMin);
            mDataMatrix[mRowCount-2][mColumnCount-2] = std::max(mDataMatrix[mRowCount-2][mColumnCount-2], rhsMax);
            for (int c=0; c<mColumnCount-2; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                mDataMatrix[mRowCount-1][c] = std::min(mDataMatrix[mRowCount-1][c], mDataMatrix[minRow][c]);
                mDataMatrix[mRowCount-2][c] = std::max(mDataMatrix[mRowCount-2][c], mDataMatrix[maxRow][c]);
                eqnMin = std::min(eqnMin, mDataMatrix[minRow][c]);
                eqnMax = std::max(eqnMax, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            mDataMatrix[minRow][mColumnCount-1] = eqnMin;
            mDataMatrix[maxRow][mColumnCount-1] = eqnMax;
            for (int c=mColumnCount-2; c<mColumnCount; ++c) {
                mDataMinimum = std::min(mDataMinimum, mDataMatrix[minRow][c]);
                mDataMaximum = std::max(mDataMaximum, mDataMatrix[maxRow][c]);
                setEmtpyCell(minRow, c);
                setEmtpyCell(maxRow, c);
            }
            minRow += 2;
            maxRow += 2;
        }
        setEmtpyCell(mRowCount-2, mColumnCount-2);
        setEmtpyCell(mRowCount-1, mColumnCount-2);
        mDataMatrix[mRowCount-2][mColumnCount-1] = 0.0;
        mDataMatrix[mRowCount-1][mColumnCount-1] = 0.0;
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        mDataHandler->setModelMinimum(mDataMinimum);
        mDataHandler->setModelMaximum(mDataMaximum);
    }

    void setEmtpyCell(int row, int column)
    {
        if (mDataMatrix[row][column] == std::numeric_limits<double>::lowest() ||
                mDataMatrix[row][column] == std::numeric_limits<double>::max()) {
            mDataMatrix[row][column] = 0.0;
        }
    }

private:
    double** mDataMatrix;
    DataHandler::CoefficientCount& mCoeffCount;
};

class SymbolsDataProvider final : public DataHandler::AbstractDataProvider
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
    SymbolsDataProvider(DataHandler *dataHandler,
                        AbstractModelInstance& modelInstance,
                        QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
    {
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
    }

    SymbolsDataProvider(const SymbolsDataProvider& other)
        : DataHandler::AbstractDataProvider(other)
    {
        mRows = new SymbolRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
        mColumnEntryCount = new int[mColumnCount];
        std::copy(other.mColumnEntryCount, other.mColumnEntryCount+other.mColumnCount, mColumnEntryCount);
    }

    SymbolsDataProvider(SymbolsDataProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mRows(other.mRows)
        , mColumnEntryCount(other.mColumnEntryCount)
    {
        other.mRowCount = 0;
        other.mColumnCount = 0;
        other.mRows = nullptr;
        other.mColumnEntryCount = nullptr;
    }

    ~SymbolsDataProvider()
    {
        if (mColumnEntryCount) delete [] mColumnEntryCount;
        if (mRows) delete [] mRows;
    }

    void loadData() override
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

    int columnEntries(int column) const override
    {
        return column < mColumnCount ? mColumnEntryCount[column] : 0;
    }

    int rowEntries(int row) const override
    {
        return row < mRowCount ? mRows[row].entries() : 0;
    }

    auto& operator=(const SymbolsDataProvider& other)
    {
        delete [] mRows;
        delete [] mColumnEntryCount;
        mRows = new SymbolRow[mRowCount];
        std::copy(other.mRows, other.mRows+other.mRowCount, mRows);
        mColumnEntryCount = new int[mColumnCount];
        std::copy(other.mColumnEntryCount, other.mColumnEntryCount+other.mColumnCount, mColumnEntryCount);
        return *this;
    }

    auto& operator=(SymbolsDataProvider&& other) noexcept
    {
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
    SymbolRow* mRows = nullptr;
    int* mColumnEntryCount = nullptr;
};

class BPOverviewDataProvider final : public DataHandler::AbstractDataProvider
{
public:
    BPOverviewDataProvider(DataHandler *dataHandler,
                           AbstractModelInstance& modelInstance,
                           QSharedPointer<AbstractViewConfiguration> viewConfig,
                           DataHandler::CoefficientCount& negPosCount)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
        , mCoeffCount(negPosCount)
    {
        mSymbolRowCount = mModelInstance.equationCount();
        mRowCount = mSymbolRowCount + 1;
        mSymbolColumnCount = mModelInstance.variableCount();
        mColumnCount = mSymbolColumnCount + 2;
        mDataMatrix = new char*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new char[mColumnCount];
            std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount, 0);
        }
    }

    BPOverviewDataProvider(const BPOverviewDataProvider& other)
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = new char*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new char[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
    }

    BPOverviewDataProvider(BPOverviewDataProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
    }

    ~BPOverviewDataProvider()
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
    }

    void loadData() override
    {
        int negRow = 1, posRow = 0;
        for (const auto& equations : mModelInstance.equations()) {
            mLogicalSectionMapping[Qt::Vertical].append(equations->firstSection());
        }
        for (const auto& variable : mModelInstance.variables()) {
            mLogicalSectionMapping[Qt::Horizontal].append(variable->firstSection());
        }
        for (int r=0; r<mModelInstance.equationCount(); ++r, negRow += 2, posRow += 2) {
            for (int c=0; c<mColumnCount-2; ++c) {
                if (mCoeffCount.count()[negRow][c] == 0 && mCoeffCount.count()[posRow][c] == 0) {
                    mDataMatrix[r][c] = 0x0;
                } else if (mCoeffCount.count()[negRow][c] == 0 && mCoeffCount.count()[posRow][c] > 0) {
                    mDataMatrix[r][c] = '+';
                } else if (mCoeffCount.count()[negRow][c] < 0 && mCoeffCount.count()[posRow][c] == 0) {
                    mDataMatrix[r][c] = '-';
                } else {
                    mDataMatrix[r][c] = 'm';
                }
            }
            mDataMatrix[r][mColumnCount-2] = mCoeffCount.count()[posRow][mCoeffCount.columnCount()-2];
            if (mCoeffCount.count()[negRow][mColumnCount-1] == 0 &&
                mCoeffCount.count()[posRow][mColumnCount-1] == 0) {
                mDataMatrix[r][mColumnCount-1] = '0';
            } else if (mCoeffCount.count()[negRow][mColumnCount-1] == 0 &&
                       mCoeffCount.count()[posRow][mColumnCount-1] > 0) {
                mDataMatrix[r][mColumnCount-1] = '+';
            } else if (mCoeffCount.count()[negRow][mColumnCount-1] < 0 &&
                       mCoeffCount.count()[posRow][mColumnCount-1] == 0) {
                mDataMatrix[r][mColumnCount-1] = '-';
            } else {
                mDataMatrix[r][mColumnCount-1] = 'm';
            }
        }
        int varColumn = 0;
        auto columns = mModelInstance.variableRowCount();
        double* lowerBounds = new double[columns];
        double* upperBounds = new double[columns];
        mModelInstance.variableLowerBounds(lowerBounds);
        mModelInstance.variableUpperBounds(upperBounds);
        for (const auto& variable : mModelInstance.variables()) {
            auto lower = std::numeric_limits<double>::max();
            auto upper = std::numeric_limits<double>::lowest();
            for (int i=variable->firstSection(); i<variable->lastSection(); ++i) {
                lower = std::min(lower, lowerBounds[i]);
                upper = std::max(upper, upperBounds[i]);
            }
            if (mModelInstance.variableType(variable->firstSection()) == 'x') { // x = continuous
                if (lower >= 0 && upper >= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '+';
                } else if (lower <= 0 && upper <= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '-';
                } else {
                    mDataMatrix[mRowCount-1][varColumn] = 'u';
                }
            } else {
                mDataMatrix[mRowCount-1][varColumn] = mModelInstance.variableType(variable->firstSection());
            }
            ++varColumn;
        }
        delete [] lowerBounds;
        delete [] upperBounds;
    }

    double data(int row, int column) const override
    {
        return mDataMatrix[row][column];
    }

    auto& operator=(const BPOverviewDataProvider& other)
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
        mDataMatrix = new char*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new char[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

    auto& operator=(BPOverviewDataProvider&& other) noexcept
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

private:
    char** mDataMatrix;
    DataHandler::CoefficientCount& mCoeffCount;
};

class BPCountDataProvider final : public DataHandler::AbstractDataProvider
{
public:
    BPCountDataProvider(DataHandler *dataHandler,
                        AbstractModelInstance& modelInstance,
                        QSharedPointer<AbstractViewConfiguration> viewConfig,
                        DataHandler::CoefficientCount& negPosCount)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
        , mCoeffCount(negPosCount)
    {
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
        mSymbolRowCount = mModelInstance.equationCount() * 2;
        mRowCount = mSymbolRowCount + 4;
        mSymbolColumnCount = mModelInstance.variableCount();
        mColumnCount = mSymbolColumnCount + 4;
        mDataMatrix = new int*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new int[mColumnCount];
            for (int c=0; r<mCoeffCount.rowCount() && c<mCoeffCount.columnCount(); ++c) {
                mDataMatrix[r][c] = mCoeffCount.count()[r][c];
                if (c != mColumnCount-4) {
                    mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[r][c]));
                    mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[r][c]));
                }
            }
            if (r < mCoeffCount.rowCount())
                std::fill(mDataMatrix[r]+mCoeffCount.columnCount(), mDataMatrix[r]+mColumnCount, 0);
            else
                std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount, 0);
        }
    }

    BPCountDataProvider(const BPCountDataProvider& other)
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = new int*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new int[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
    }

    BPCountDataProvider(BPCountDataProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
    }

    ~BPCountDataProvider()
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
    }

    void loadData() override
    {
        for (const auto& equation : mModelInstance.equations()) {
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
        }
        for (const auto& variable : mModelInstance.variables()) {
            mLogicalSectionMapping[Qt::Horizontal].append(variable->firstSection());
        }
        for (int r=0, negRow = 1, posRow = 0; r<mModelInstance.equationCount(); ++r, negRow += 2, posRow += 2) {
            for (int v=0; v<mModelInstance.variableCount(); ++v) {
                mDataMatrix[negRow][mColumnCount-2] += mDataMatrix[negRow][v];
                mDataMatrix[posRow][mColumnCount-2] += mDataMatrix[posRow][v];
                mDataMatrix[mRowCount-3][v] += mDataMatrix[negRow][v];
                mDataMatrix[mRowCount-4][v] += mDataMatrix[posRow][v];
            }
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[negRow][mColumnCount-2]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[negRow][mColumnCount-2]));
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[posRow][mColumnCount-2]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[posRow][mColumnCount-2]));
            mDataMatrix[mRowCount-3][mColumnCount-3] += mDataMatrix[negRow][mColumnCount-3];
            mDataMatrix[mRowCount-4][mColumnCount-3] += mDataMatrix[posRow][mColumnCount-3];
            mDataMatrix[mRowCount-3][mColumnCount-2] += mDataMatrix[negRow][mColumnCount-2];
            mDataMatrix[mRowCount-4][mColumnCount-2] += mDataMatrix[posRow][mColumnCount-2];
        }
        for (int r=mRowCount-4; r<mRowCount-1; ++r) {
            for (int c=0; c<mColumnCount; ++c) {
                mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[r][c]));
                mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[r][c]));
            }
        }
        int index = 0;
        for (const auto& equation : mModelInstance.equations()) {
            mDataMatrix[index][mColumnCount-1] = equation->entries();
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[index][mColumnCount-1]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[index][mColumnCount-1]));
            index += 2;
        }
        index = 0;
        for (const auto& variable : mModelInstance.variables()) {
            mDataMatrix[mRowCount-2][index] = variable->entries();
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[mRowCount-2][index]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[mRowCount-2][index]));
            ++index;
        }
        int varColumn = 0;
        auto columns = mModelInstance.variableRowCount();
        double* lowerBounds = new double[columns];
        double* upperBounds = new double[columns];
        mModelInstance.variableLowerBounds(lowerBounds);
        mModelInstance.variableUpperBounds(upperBounds);
        for (const auto& variable : mModelInstance.variables()) {
            auto lower = std::numeric_limits<double>::max();
            auto upper = std::numeric_limits<double>::lowest();
            for (int i=variable->firstSection(); i<variable->lastSection(); ++i) {
                lower = std::min(lower, lowerBounds[i]);
                upper = std::max(upper, upperBounds[i]);
            }
            if (mModelInstance.variableType(variable->firstSection()) == 'x') { // x = continuous
                if (lower >= 0 && upper >= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '+';
                } else if (lower <= 0 && upper <= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '-';
                } else {
                    mDataMatrix[mRowCount-1][varColumn] = 'u';
                }
            } else {
                mDataMatrix[mRowCount-1][varColumn] = mModelInstance.variableType(variable->firstSection());
            }
            ++varColumn;
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        delete [] lowerBounds;
        delete [] upperBounds;
    }

    double data(int row, int column) const override
    {
        return mDataMatrix[row][column];
    }

    auto& operator=(const BPCountDataProvider& other)
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
        mDataMatrix = new int*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new int[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

    auto& operator=(BPCountDataProvider&& other) noexcept
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

private:
    int** mDataMatrix;
    DataHandler::CoefficientCount& mCoeffCount;
};

class BPAverageDataProvider final : public DataHandler::AbstractDataProvider
{
public:
    BPAverageDataProvider(DataHandler *dataHandler,
                          AbstractModelInstance& modelInstance,
                          QSharedPointer<AbstractViewConfiguration> viewConfig,
                          DataHandler::CoefficientCount& negPosCount)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
        , mCoeffCount(negPosCount)
    {
        mDataMinimum = std::numeric_limits<double>::max();
        mDataMaximum = std::numeric_limits<double>::lowest();
        mSymbolRowCount = mModelInstance.equationCount() * 2;
        mRowCount = mSymbolRowCount + 4;
        mSymbolColumnCount = mModelInstance.variableCount();
        mColumnCount = mSymbolColumnCount + 4;
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::fill(mDataMatrix[r], mDataMatrix[r]+mColumnCount, 0);
        }
    }

    BPAverageDataProvider(const BPAverageDataProvider& other)
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
    }

    BPAverageDataProvider(BPAverageDataProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mCoeffCount(other.mCoeffCount)
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
    }

    ~BPAverageDataProvider()
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
    }

    void loadData() override
    {
        for (const auto& equation : mModelInstance.equations()) {
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
            mLogicalSectionMapping[Qt::Vertical].append(equation->firstSection());
        }
        for (const auto& variable : mModelInstance.variables()) {
            mLogicalSectionMapping[Qt::Horizontal].append(variable->firstSection());
        }
        int index = 0;
        for (const auto& equation : mModelInstance.equations()) {
            mDataMatrix[index][mColumnCount-1] = equation->entries();
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[index][mColumnCount-1]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[index][mColumnCount-1]));
            index += 2;
        }
        index = 0;
        for (const auto& variable : mModelInstance.variables()) {
            mDataMatrix[mRowCount-2][index] = variable->entries();
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[mRowCount-2][index]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[mRowCount-2][index]));
            ++index;
        }
        for (int r=0, negRow = 1, posRow = 0; r<mModelInstance.equationCount(); ++r, negRow += 2, posRow += 2) {
            for (int c=0; c<mColumnCount-4; ++c) {
                mDataMatrix[negRow][c] = mCoeffCount.count()[negRow][c] / mDataMatrix[mRowCount-2][c];
                mDataMatrix[negRow][mColumnCount-2] += mCoeffCount.count()[negRow][c];
                mDataMatrix[mRowCount-3][c] += mCoeffCount.count()[negRow][c];
                mDataMatrix[posRow][c] = mCoeffCount.count()[posRow][c] / mDataMatrix[mRowCount-2][c];
                mDataMatrix[posRow][mColumnCount-2] += mCoeffCount.count()[posRow][c];
                mDataMatrix[mRowCount-4][c] += mCoeffCount.count()[posRow][c];
                mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[negRow][c]));
                mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[posRow][c]));
                mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[negRow][c]));
                mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[posRow][c]));
            }
            for (int c=mCoeffCount.columnCount()-2;
                 posRow<mCoeffCount.rowCount() && c<mCoeffCount.columnCount(); ++c) {
                mDataMatrix[posRow][c] = mCoeffCount.count()[posRow][c];
                if (c != mColumnCount-4) {
                    mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[posRow][c]));
                    mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[posRow][c]));
                }
            }
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[posRow][mColumnCount-1]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[posRow][mColumnCount-1]));
            mDataMatrix[negRow][mColumnCount-2] /= mDataMatrix[posRow][mColumnCount-1];
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[negRow][mColumnCount-2]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[negRow][mColumnCount-2]));
            mDataMatrix[posRow][mColumnCount-2] /= mDataMatrix[posRow][mColumnCount-1];
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[negRow][mColumnCount-2]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[negRow][mColumnCount-2]));
        }
        for (int c=0; c<mColumnCount-4; ++c) {
            mDataMatrix[mRowCount-3][c] /= mDataMatrix[mRowCount-2][c];
            mDataMatrix[mRowCount-4][c] /= mDataMatrix[mRowCount-2][c];
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[mRowCount-3][c]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[mRowCount-4][c]));
            mDataMinimum = std::min(mDataMinimum, double(mDataMatrix[mRowCount-3][c]));
            mDataMaximum = std::max(mDataMaximum, double(mDataMatrix[mRowCount-4][c]));
        }
        int varColumn = 0;
        auto columns = mModelInstance.variableRowCount();
        double* lowerBounds = new double[columns];
        double* upperBounds = new double[columns];
        mModelInstance.variableLowerBounds(lowerBounds);
        mModelInstance.variableUpperBounds(upperBounds);
        for (const auto& variable : mModelInstance.variables()) {
            auto lower = std::numeric_limits<double>::max();
            auto upper = std::numeric_limits<double>::lowest();
            for (int i=variable->firstSection(); i<variable->lastSection(); ++i) {
                lower = std::min(lower, lowerBounds[i]);
                upper = std::max(upper, upperBounds[i]);
            }
            if (mModelInstance.variableType(variable->firstSection()) == 'x') { // x = continuous
                if (lower >= 0 && upper >= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '+';
                } else if (lower <= 0 && upper <= 0) {
                    mDataMatrix[mRowCount-1][varColumn] = '-';
                } else {
                    mDataMatrix[mRowCount-1][varColumn] = 'u';
                }
            } else {
                mDataMatrix[mRowCount-1][varColumn] = mModelInstance.variableType(variable->firstSection());
            }
            ++varColumn;
        }
        mViewConfig->defaultValueFilter().MinValue = mDataMinimum;
        mViewConfig->defaultValueFilter().MaxValue = mDataMaximum;
        mViewConfig->currentValueFilter().MinValue = mDataMinimum;
        mViewConfig->currentValueFilter().MaxValue = mDataMaximum;
        delete [] lowerBounds;
        delete [] upperBounds;
    }

    double data(int row, int column) const override
    {
        return mDataMatrix[row][column];
    }

    auto& operator=(const BPAverageDataProvider& other)
    {
        for (int r=0; r<mRowCount; ++r) {
            delete [] mDataMatrix[r];
        }
        delete [] mDataMatrix;
        mDataMatrix = new double*[mRowCount];
        for (int r=0; r<mRowCount; ++r) {
            mDataMatrix[r] = new double[mColumnCount];
            std::copy(other.mDataMatrix[r], other.mDataMatrix[r]+other.mColumnCount, mDataMatrix[r]);
        }
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

    auto& operator=(BPAverageDataProvider&& other) noexcept
    {
        mDataMatrix = other.mDataMatrix;
        other.mDataMatrix = nullptr;
        mCoeffCount = other.mCoeffCount;
        return *this;
    }

private:
    double** mDataMatrix;
    DataHandler::CoefficientCount& mCoeffCount;
};

class PostoptDataProvider final : public DataHandler::AbstractDataProvider
{
public:
    PostoptDataProvider(DataHandler *dataHandler,
                        AbstractModelInstance& modelInstance,
                        QSharedPointer<AbstractViewConfiguration> viewConfig)
        : DataHandler::AbstractDataProvider(dataHandler, modelInstance, viewConfig)
    {
        mColumnCount = 5;
        mRootItem = QSharedPointer<PostoptTreeItem>(new LinePostoptTreeItem());
        value = std::bind(&PostoptDataProvider::identity, this, std::placeholders::_1);
    }

    PostoptDataProvider(const PostoptDataProvider& other)
        : DataHandler::AbstractDataProvider(other)
        , mRootItem(other.mRootItem)
    {

    }

    PostoptDataProvider(PostoptDataProvider&& other) noexcept
        : DataHandler::AbstractDataProvider(other)
        , mRootItem(other.mRootItem)
    {
        other.mRootItem = nullptr;
    }

    ~PostoptDataProvider()
    {

    }

    void loadData() override
    {
        if (mViewConfig->currentValueFilter().isAbsolute()) {
            value = std::bind(&PostoptDataProvider::abs, this, std::placeholders::_1);
        } else {
            value = std::bind(&PostoptDataProvider::identity, this, std::placeholders::_1);
        }
        mRootItem = QSharedPointer<PostoptTreeItem>(new LinePostoptTreeItem());
        auto equations = new GroupPostoptTreeItem(PostoptTreeItem::EquationHeader, mRootItem.get());
        mRootItem->append(equations);
        for (auto equation : mModelInstance.equations()) {
            auto eqnGroup = new GroupPostoptTreeItem(equation->name(), equations);
            equations->append(eqnGroup);
            for (int e=0; e<equation->entries(); ++e) {
                auto eqnLine = new GroupPostoptTreeItem(symbolName(equation, e), eqnGroup);
                eqnGroup->append(eqnLine);
                loadAttributes(equation, e, eqnLine);
                loadVariables(equation, e, eqnLine);
            }
        }
        auto variables = new GroupPostoptTreeItem(PostoptTreeItem::VariableHeader, mRootItem.get());
        mRootItem->append(variables);
        for (auto variable : mModelInstance.variables()) {
            auto varGroup = new GroupPostoptTreeItem(variable->name(), variables);
            variables->append(varGroup);
            for (int e=0; e<variable->entries(); ++e) {
                auto varLine = new GroupPostoptTreeItem(symbolName(variable, e), varGroup);
                varGroup->append(varLine);
                loadAttributes(variable, e, varLine);
                loadEquations(variable, e, varLine);
            }
        }
    }

    double data(int row, int column) const override
    {
        Q_UNUSED(row);
        Q_UNUSED(column);
        return 0.0;
    }

    QSharedPointer<PostoptTreeItem> dataTree() const
    {
        return mRootItem;
    }

    auto& operator=(const PostoptDataProvider& other)
    {
        mRootItem = other.mRootItem;
        return *this;
    }

    auto& operator=(PostoptDataProvider&& other) noexcept
    {
        mRootItem = other.mRootItem;
        other.mRootItem = nullptr;
        return *this;
    }

private:
    void loadAttributes(Symbol *symbol, int entry, PostoptTreeItem *parent)
    {
        bool abs = mViewConfig->currentValueFilter().isAbsolute();
        auto attributes = new GroupPostoptTreeItem(PostoptTreeItem::AttributeHeader, parent);
        parent->append(attributes);
        for (const auto& label : PostoptTreeItem::AttributeRowHeader) {
            QVariant value;
            if (symbol->isEquation()) {
                value = mModelInstance.equationAttribute(label.toString(), symbol->firstSection(), entry, abs);
            } else if (symbol->isVariable()) {
                value = mModelInstance.variableAttribute(label.toString(), symbol->firstSection(), entry, abs);
            }
            attributes->append(new LinePostoptTreeItem({label, value}, attributes));
        }
    }

    void loadEquations(Symbol *variable, int entry, PostoptTreeItem *parent)
    {
        bool abs = mViewConfig->currentValueFilter().isAbsolute();
        auto equations = new LinePostoptTreeItem(PostoptTreeItem::EquationLineHeader, parent);
        parent->append(equations);
        for (auto equation : mModelInstance.equations()) {
            auto eqnGroup = new GroupPostoptTreeItem(equation->name());
            for (int e=0; e<equation->entries(); ++e) {
                auto row = dataRow(equation->firstSection()+e);
                QVariant jacval;
                if (row) {
                    jacval = row->value(variable->firstSection()+entry, variable->lastSection());
                }
                if (jacval.isValid()) {
                    auto name = symbolName(equation, e);
                    double jac = value(jacval.toDouble());
                    double xi = mModelInstance.equationAttribute(Mi::MarginalNum, equation->firstSection(), e, abs).toDouble();
                    double jacxi = value(jacval.toDouble() * xi);
                    eqnGroup->append(new LinePostoptTreeItem({name, jac, xi, jacxi}, eqnGroup));
                }
            }
            if (eqnGroup->rowCount()) {
                eqnGroup->setParent(equations);
                equations->append(eqnGroup);
            } else {
                delete eqnGroup;
            }
        }
    }

    void loadVariables(Symbol *equation, int entry, PostoptTreeItem *parent)
    {
        bool abs = mViewConfig->currentValueFilter().isAbsolute();
        auto variables = new LinePostoptTreeItem(PostoptTreeItem::VariableLineHeader, parent);
        parent->append(variables);
        for (auto variable : mModelInstance.variables()) {
            auto varGroup = new GroupPostoptTreeItem(variable->name());
            for (int e=0; e<variable->entries(); ++e) {
                auto row = dataRow(equation->firstSection()+entry);
                QVariant jacval;
                if (row) {
                    jacval = row->value(variable->firstSection()+e, variable->lastSection());
                }
                if (jacval.isValid()) {
                    auto name = symbolName(variable, e);
                    double jac = value(jacval.toDouble());
                    double ui = mModelInstance.variableAttribute(Mi::Level, variable->firstSection(), e, abs).toDouble();
                    double jacui = value(jac * ui);
                    varGroup->append(new LinePostoptTreeItem({name, jac, ui, jacui}, varGroup));
                }
            }
            if (varGroup->rowCount()) {
                varGroup->setParent(variables);
                variables->append(varGroup);
            } else {
                delete varGroup;
            }
        }
    }

    QString symbolName(Symbol *symbol, int entry)
    {
        if (symbol->isScalar())
            return symbol->name();
        int index = symbol->firstSection()+entry;
        if (!symbol->sectionLabels().contains(index))
            return QString("(..)");
        return QString("%1(%2)").arg(symbol->name(), symbol->sectionLabels()[index].join(", "));
    }

    double abs(double value)
    {
        return std::abs(value);
    }

    double identity(double value)
    {
        return value;
    }

private:
    QSharedPointer<PostoptTreeItem> mRootItem;
    std::function<double(double)> value;
};

DataHandler::DataHandler(AbstractModelInstance& modelInstance)
    : mModelInstance(modelInstance)
{

}

DataHandler::~DataHandler()
{
    if (mCoeffCount) delete mCoeffCount;
}

void DataHandler::aggregate(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    if (!viewConfig) return;
    auto provider = newProvider(viewConfig);
    mDataCache.remove(viewConfig->viewId());
    if (viewConfig->currentAggregation().type() != Aggregation::None) {
        provider->loadData();
    } else {
        return;
    }
    mDataCache[viewConfig->viewId()] = provider;
}

void DataHandler::loadData(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    if (!viewConfig) return;
    auto provider = newProvider(viewConfig);
    mDataCache.remove(viewConfig->viewId());
    provider->loadData();
    mDataCache[viewConfig->viewId()] = provider;
}

QVariant DataHandler::data(int row, int column, int viewId) const
{
    if (mDataCache.contains(viewId) && mDataCache[viewId]->data(row, column) != 0.0) {
        return mDataCache[viewId]->data(row, column);
    }
    return QVariant();
}

QSharedPointer<PostoptTreeItem> DataHandler::dataTree(int viewId) const
{
    if (mDataCache.contains(viewId)) {
        auto provider = static_cast<PostoptDataProvider*>(mDataCache[viewId].get());
        return provider->dataTree();
    }
    return nullptr;
}

void DataHandler::remove(int viewId)
{
    mDataCache.remove(viewId);
}

int DataHandler::headerData(int logicalIndex,
                            Qt::Orientation orientation,
                            int viewId) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->headerData(orientation, logicalIndex)
                                       : -1;
}

QVariant DataHandler::plainHeaderData(Qt::Orientation orientation,
                                      int viewId, int logicalIndex,
                                      int dimension) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->plainHeaderData(orientation, logicalIndex, dimension)
                                       : QVariant();
}

QVariant DataHandler::sectionLabels(Qt::Orientation orientation, int viewId, int logicalIndex) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->sectionLabels(orientation, logicalIndex)
                                       : QStringList();
}

int DataHandler::rowCount(int viewId) const
{
    if (mDataCache.contains(viewId)) {
        return mDataCache[viewId]->rowCount();
    }
    return 0;
}

int DataHandler::rowEntries(int row, int viewId) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->rowEntries(row) : 0;
}

int DataHandler::columnCount(int viewId) const
{
    if (mDataCache.contains(viewId)) {
        return mDataCache[viewId]->columnCount();
    }
    return 0;
}

int DataHandler::columnEntries(int column, int viewId) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->columnEntries(column) : 0;
}

int DataHandler::symbolRowCount(int viewId) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->symbolRowCount() : 0;
}

int DataHandler::symbolColumnCount(int viewId) const
{
    return mDataCache.contains(viewId) ? mDataCache[viewId]->symbolColumnCount() : 0;
}

double DataHandler::modelMinimum() const
{
    return mModelMinimum;
}

void DataHandler::setModelMinimum(double minimum)
{
    mModelMinimum = minimum;
}

double DataHandler::modelMaximum() const
{
    return mModelMaximum;
}

void DataHandler::setModelMaximum(double maximum)
{
    mModelMaximum = maximum;
}

QSharedPointer<AbstractViewConfiguration> DataHandler::clone(int viewId, int newView)
{
    if (!mDataCache.contains(viewId))
        return nullptr;
    mDataCache[newView] = QSharedPointer<AbstractDataProvider>(cloneProvider(viewId));
    mDataCache[newView]->viewConfig()->setViewId(newView);
    return mDataCache[newView]->viewConfig();
}

void DataHandler::loadJacobian()
{
    mDataMatrix = DataMatrix(mModelInstance.equationRowCount());
    mModelInstance.jacobianData(mDataMatrix);
}

DataHandler::AbstractDataProvider* DataHandler::cloneProvider(int viewId)
{
    switch (mDataCache[viewId]->viewConfig()->viewType()) {
    case ViewDataType::BP_Scaling:
    {
        auto provider = static_cast<BPScalingProvider*>(mDataCache[viewId].get());
        return new BPScalingProvider(*provider);
    }
    case ViewDataType::Symbols:
    {
        auto provider = static_cast<SymbolsDataProvider*>(mDataCache[viewId].get());
        return new SymbolsDataProvider(*provider);
    }
    case ViewDataType::BP_Overview:
    {
        auto provider = static_cast<BPOverviewDataProvider*>(mDataCache[viewId].get());
        return new BPOverviewDataProvider(*provider);
    }
    case ViewDataType::BP_Count:
    {
        auto provider = static_cast<BPCountDataProvider*>(mDataCache[viewId].get());
        return new BPCountDataProvider(*provider);
    }
    case ViewDataType::BP_Average:
    {
        auto provider = static_cast<BPAverageDataProvider*>(mDataCache[viewId].get());
        return new BPAverageDataProvider(*provider);
    }
    case ViewDataType::Postopt:
    {
        auto provider = static_cast<PostoptDataProvider*>(mDataCache[viewId].get());
        return new PostoptDataProvider(*provider);
    }
    default:
    {
        auto provider = static_cast<IdentityDataProvider*>(mDataCache[viewId].get());
        return new IdentityDataProvider(*provider);
    }
    }
}

QSharedPointer<DataHandler::AbstractDataProvider> DataHandler::newProvider(QSharedPointer<AbstractViewConfiguration> viewConfig)
{
    if (!mCoeffCount) {
        mCoeffCount = new CoefficientCount(mModelInstance.variableCount()+2,
                                           mModelInstance.equationCount()*2);
    }
    switch (viewConfig->viewType()) {
    case ViewDataType::BP_Scaling:
        return QSharedPointer<AbstractDataProvider>(new BPScalingProvider(this,
                                                                           mModelInstance,
                                                                           viewConfig,
                                                                           *mCoeffCount));
    case ViewDataType::Symbols:
        return QSharedPointer<AbstractDataProvider>(new SymbolsDataProvider(this,
                                                                            mModelInstance,
                                                                            viewConfig));
    case ViewDataType::BP_Overview:
        return QSharedPointer<AbstractDataProvider>(new BPOverviewDataProvider(this,
                                                                               mModelInstance,
                                                                               viewConfig,
                                                                               *mCoeffCount));
    case ViewDataType::BP_Count:
        return QSharedPointer<AbstractDataProvider>(new BPCountDataProvider(this,
                                                                            mModelInstance,
                                                                            viewConfig,
                                                                            *mCoeffCount));
    case ViewDataType::BP_Average:
        return QSharedPointer<AbstractDataProvider>(new BPAverageDataProvider(this,
                                                                              mModelInstance,
                                                                              viewConfig,
                                                                              *mCoeffCount));
    case ViewDataType::Postopt:
        return QSharedPointer<AbstractDataProvider>(new PostoptDataProvider(this,
                                                                            mModelInstance,
                                                                            viewConfig));
    default:
        return QSharedPointer<AbstractDataProvider>(new IdentityDataProvider(this,
                                                                             mModelInstance,
                                                                             viewConfig));
    }
}

}
}
}
