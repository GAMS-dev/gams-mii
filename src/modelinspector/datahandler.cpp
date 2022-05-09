#include "datahandler.h"
#include "modelinstance.h"

#include <algorithm>
#include <functional>
#include <vector>

#include <QSet>

using namespace std;

namespace gams {
namespace studio {
namespace modelinspector {

class DataHandler::AbstractDataAggregator
{
protected:
    AbstractDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : mAppliedAggregation(appliedAggregation)
        , mDataHandler(dataHandler)
    {
    }

public:
    virtual ~AbstractDataAggregator()
    {
    }

    const Aggregation& aggregation() const
    {
        return mAppliedAggregation;
    }

    virtual void aggregateColumns(ModelInstance *modelInstance) = 0;

    virtual void aggregateRows(ModelInstance *modelInstance) = 0;

    DataMatrix& aggregatedMatrix()
    {
        return mAggrMatrix;
    }

    const AggregationMap& aggregationMap() const
    {
        return mAppliedAggregation.aggregationMap();
    }

    bool useAbsoluteValues() const
    {
        return mAppliedAggregation.useAbsoluteValues();
    }

    void setFilterForTargetIndexChecked(Qt::Orientation orientation,
                                        int symIndex, int targetIndex)
    {
        mAppliedAggregation.setLabelState(orientation,
                                          symIndex,
                                          targetIndex,
                                          Qt::Checked);
    }

protected:
    virtual QVariant aggregatedValue(QVariant &value1, QVariant &value2)
    {
        if (isSpecialValue(value1))
            return QVariant();
        value2 = QVariant();
        auto value = value1.toDouble();
        if (value < 0 || value > 0) {
            return value;
        }
        return QVariant();
    }

    virtual QVariant aggregatedAbsValue(QVariant &value1, QVariant &value2)
    {
        if (isSpecialValue(value1))
            return QVariant();
        value2 = QVariant();
        auto value = value1.toDouble();
        if (value < 0 || value > 0) {
            return std::abs(value);
        }
        return QVariant();
    }

    bool isSpecialValue(const QVariant &value) const
    {
        auto str = value.toString();
        return !str.compare(EPS, Qt::CaseInsensitive) ||
                !str.compare(P_INF, Qt::CaseInsensitive) ||
                !str.compare(N_INF, Qt::CaseInsensitive) ||
                !str.compare(NA, Qt::CaseInsensitive);
    }

protected:
    Aggregation mAppliedAggregation;
    DataHandler *mDataHandler;
    DataMatrix mAggrMatrix;
};

class IdentityDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    IdentityDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler, Aggregation())
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        Q_UNUSED(modelInstance);
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        Q_UNUSED(modelInstance);
    }
};

class CountDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    CountDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                DataRow newRow = iter->second;
                Q_FOREACH(auto sections, item.unitedSections()) {
                    QVariant value;
                    Q_FOREACH(auto column, sections) {
                        value = aggregatedValue(value, newRow[column]);
                        newRow.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    newRow[targetColumn++] = value;
                }
                aggregatedMatrix()[iter->first] = newRow;
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column) {
                    QVariant value;
                    Q_FOREACH(auto row, sections) {
                        value = aggregatedValue(value, aggregatedMatrix()[row][column]);
                    }
                    if (value.isValid()) newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }

protected:
    QVariant aggregatedValue(QVariant &value1, QVariant &value2) override
    {
        if (!value1.isValid() && value2.isValid())
            return 1;
        if (value2.isValid()) {
            int value = value1.toInt();
            return value + 1;
        }
        return value1;
    }
};

class MeanDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MeanDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MeanDataAggregator::aggregatedAbsValue
                                            : &MeanDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                Q_FOREACH(auto sections, item.unitedSections()) {
                    int count = 0;
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto column, sections) {
                        auto ret = std::invoke(getValue, this, iter->second[column], specialValue);
                        if (ret.isValid()) {
                            ++count;
                            value += ret.toDouble();
                        }
                        iter->second.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    if (count) iter->second[targetColumn++] = value / count;
                    else iter->second[targetColumn++] = specialValue;
                }
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MeanDataAggregator::aggregatedAbsValue
                                            : &MeanDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0, count = 0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column, count = 0) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            ++count;
                            value += ret.toDouble();
                        }
                    }
                    if (count) newRow[column] = value / count;
                    else newRow[column] = specialValue;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class MedianDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MedianDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MedianDataAggregator::aggregatedAbsValue
                                            : &MedianDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                Q_FOREACH(auto sections, item.unitedSections()) {
                    QVariant value = NA;
                    vector<double> values;
                    Q_FOREACH(auto column, sections) {
                        auto ret = std::invoke(getValue, this, iter->second[column], value);
                        if (ret.isValid())
                            values.push_back(ret.toDouble());
                        iter->second.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    if (value.isValid()) iter->second[targetColumn++] = value;
                    else iter->second[targetColumn++] = median(values);
                }
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MedianDataAggregator::aggregatedAbsValue
                                            : &MedianDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column) {
                    QVariant value = NA;
                    vector<double> values;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedMatrix()[row][column], value);
                        if (ret.isValid())
                            values.push_back(ret.toDouble());
                    }
                    if (value.isValid()) newRow[column] = value;
                    else newRow[column] = median(values);
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }

protected:
    double median(vector<double> &values)
    {
        if (values.empty()) return 0.0;
        auto size = values.size();
        if (size % 2 == 0) {
            nth_element(values.begin(), values.begin() + size/2, values.end());
            nth_element(values.begin(), values.begin() + (size-1)/2, values.end());
            return (double)(values[(size-1)/2] + values[size/2]) / 2.0;
        }
        nth_element(values.begin(), values.begin() + size/2, values.end());
        return (double)values[size/2];
    }
};

class MinDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MinDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MinDataAggregator::aggregatedAbsValue
                                            : &MinDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                DataRow newRow = iter->second;
                Q_FOREACH(auto sections, item.unitedSections()) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto column, sections) {
                        auto ret = std::invoke(getValue, this, newRow[column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMin(value, ret.toDouble());
                        }
                        newRow.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    if (specialValue.isValid()) newRow[targetColumn++] = specialValue;
                    else newRow[targetColumn++] = value;
                }
                aggregatedMatrix()[iter->first] = newRow;
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MinDataAggregator::aggregatedAbsValue
                                            : &MinDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMin(value, ret.toDouble());
                        }
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class MaxDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MaxDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MaxDataAggregator::aggregatedAbsValue
                                            : &MaxDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                DataRow newRow = iter->second;
                Q_FOREACH(auto sections, item.unitedSections()) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto column, sections) {
                        auto ret = std::invoke(getValue, this, newRow[column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMax(value, ret.toDouble());
                        }
                        newRow.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    if (specialValue.isValid()) newRow[targetColumn++] = specialValue;
                    else newRow[targetColumn++] = value;
                }
                aggregatedMatrix()[iter->first] = newRow;
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MaxDataAggregator::aggregatedAbsValue
                                            : &MaxDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMax(value, ret.toDouble());
                        }
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class SumDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    SumDataAggregator(DataHandler *dataHandler, const Aggregation &appliedAggregation)
        : DataHandler::AbstractDataAggregator(dataHandler, appliedAggregation)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &SumDataAggregator::aggregatedAbsValue
                                            : &SumDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedMatrix().keyValueBegin();
                 iter!=aggregatedMatrix().keyValueEnd(); ++iter) {
                int targetColumn = var.firstSection();
                DataRow newRow = iter->second;
                Q_FOREACH(auto sections, item.unitedSections()) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto column, sections) {
                        auto ret = std::invoke(getValue, this, newRow[column], specialValue);
                        if (ret.isValid()) value += ret.toDouble();
                        newRow.remove(column);
                    }
                    setFilterForTargetIndexChecked(Qt::Horizontal, var.firstSection(), targetColumn);
                    if (specialValue.isValid()) newRow[targetColumn++] = specialValue;
                    else newRow[targetColumn++] = value;
                }
                aggregatedMatrix()[iter->first] = newRow;
            }
        }
    }

    virtual void aggregateRows(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &SumDataAggregator::aggregatedAbsValue
                                            : &SumDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Vertical]) {
            auto eqn = modelInstance->equation(item.symbolIndex());
            int targetRow = eqn.firstSection();
            Q_FOREACH(auto sections, item.unitedSections()) {
                DataRow newRow;
                for (int column=0; column<modelInstance->columnCount(PredefinedViewEnum::Full); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedMatrix()[row][column], specialValue);
                        if (ret.isValid()) value += ret.toDouble();
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedMatrix()[targetRow++] = newRow;
            }
        }
    }
};

DataHandler::DataHandler()
    : mDataAggregator(QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this)))
{
}

void DataHandler::aggregate(const Aggregation &aggregation, ModelInstance *modelInstance)
{
    setDataAggregator(aggregation);
    mLogicalSectionMapping[aggregation.view()].clear();
    mAggrCache.remove(aggregation.view());
    if (aggregation.type() == Aggregation::None)
        return;

    applyValueFilter();
    mDataAggregator->aggregateRows(modelInstance);
    applyRowFilter(modelInstance);
    setDefaultColumnValues(modelInstance->columnCount(PredefinedViewEnum::Full));
    mDataAggregator->aggregateColumns(modelInstance);
    applyColumnFilter(modelInstance);
    mAggrCache[aggregation.view()] = mDataAggregator->aggregatedMatrix();
}

QVariant DataHandler::data(int row, int column, int view) const
{
    return mAggrCache.contains(view) ? mAggrCache[view][row][column] : mDataMatrix[row][column];
}

int DataHandler::headerData(int logicalIndex,
                            Qt::Orientation orientation,
                            int view) const
{
    if (logicalIndex < mLogicalSectionMapping[view][orientation].size()) {
        return mLogicalSectionMapping[view][orientation][logicalIndex];
    }
    return -1;
}

void DataHandler::loadDataMatrix(ModelInstance *modelInstance)
{
    for (int row=0; row<modelInstance->equationCount(); ++row) {
        auto data = modelInstance->jaccobianRow(row);
        for (auto iter=data.keyValueBegin(); iter!=data.keyValueEnd(); ++iter) {
            mDataMatrix[row+PredefinedHeaderLength][iter->first+PredefinedHeaderLength] = iter->second;
        }
    }
    for (int r=0; r<PredefinedHeaderLength; ++r) {
        auto header = PredefinedHeader.at(r).toLower();
        for (int c=0; c<modelInstance->jaccColumnCount(); ++c) {
            mDataMatrix[r][c+PredefinedHeaderLength] = modelInstance->horizontalAttribute(header, c);
        }
    }
    for (int c=0; c<PredefinedHeaderLength; ++c) {
        auto header = PredefinedHeader.at(c).toLower();
        for (int r=0; r<modelInstance->jaccRowCount(); ++r) {
            mDataMatrix[r+PredefinedHeaderLength][c] = modelInstance->verticalAttribute(header, r);
        }
    }
}

void DataHandler::setDataAggregator(const Aggregation &appliedAggregation)
{
    switch (appliedAggregation.type()) {
    case Aggregation::Count:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new CountDataAggregator(this, appliedAggregation));
        break;
    case Aggregation::Mean:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MeanDataAggregator(this, appliedAggregation));
        break;
    case Aggregation::Median:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MedianDataAggregator(this, appliedAggregation));
        break;
    case Aggregation::Maximum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MaxDataAggregator(this, appliedAggregation));
        break;
    case Aggregation::Minimum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MinDataAggregator(this, appliedAggregation));
        break;
    case Aggregation::Sum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new SumDataAggregator(this, appliedAggregation));
        break;
    default:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this));
        break;
    }
}

void DataHandler::applyValueFilter()
{
    for (auto rIter=mDataMatrix.keyValueBegin(); rIter!=mDataMatrix.keyValueEnd(); ++rIter) {
        DataRow newRow;
        if (keepRow(rIter->first)) {
            Q_FOREACH(auto column, rIter->second.keys()) {
                if (mDataAggregator->aggregation().valueFilter().accepts(rIter->second[column]) && keepColumn(column))
                    newRow[column] = rIter->second[column];
            }
        }
        mDataAggregator->aggregatedMatrix()[rIter->first] = newRow;
    }
}

void DataHandler::applyRowFilter(ModelInstance *modelInstance)
{
    int row = 0;
    DataMatrix tmpMatrix;
    auto checkState = checkStates(Qt::Vertical, modelInstance);
    Q_FOREACH(auto key, mDataAggregator->aggregatedMatrix().keys()) {
        if (checkState[key] == Qt::Checked) {
            tmpMatrix[row++] = mDataAggregator->aggregatedMatrix()[key];
            mLogicalSectionMapping[mDataAggregator->aggregation().view()][Qt::Vertical].append(key);
        }
    }
    mDataAggregator->aggregatedMatrix() = tmpMatrix;
}

void DataHandler::applyColumnFilter(ModelInstance *modelInstance)
{
    auto checkState = checkStates(Qt::Horizontal, modelInstance);
    for (auto rIter=mDataAggregator->aggregatedMatrix().keyValueBegin();
         rIter!=mDataAggregator->aggregatedMatrix().keyValueEnd(); ++rIter) {
        Q_FOREACH(auto column, rIter->second.keys()) {
            if (checkState[column] == Qt::Unchecked)
                rIter->second.remove(column);
        }
        if (mLogicalSectionMapping[mDataAggregator->aggregation().view()][Qt::Horizontal].isEmpty()) {
            mLogicalSectionMapping[mDataAggregator->aggregation().view()][Qt::Horizontal] = rIter->second.keys();
        }
        int column = 0;
        DataRow newRow;
        Q_FOREACH(auto value, rIter->second.values()) {
            newRow[column++] = value;
        }
        rIter->second = newRow;
    }
}

void DataHandler::setDefaultColumnValues(int columnCount)
{
    for (auto rIter=mDataAggregator->aggregatedMatrix().keyValueBegin();
         rIter!=mDataAggregator->aggregatedMatrix().keyValueEnd(); ++rIter) {
        for (int c=0; c<columnCount; ++c) {
            if (!rIter->second.contains(c))
                rIter->second[c] = QVariant();
        }
    }
}

IndexCheckStates DataHandler::checkStates(Qt::Orientation orientation,
                                         ModelInstance *modelInstance) const
{
    IndexCheckStates states;
    Q_FOREACH(auto index, mDataAggregator->aggregation().identifierFilter()[orientation].keys()) {
        if (index < PredefinedHeaderLength) {
            states[index] = mDataAggregator->aggregation().identifierFilter()[orientation][index].Checked;
            continue;
        }
        auto symbol = orientation == Qt::Vertical ? modelInstance->equation(index)
                                                  : modelInstance->variable(index);
        if (mDataAggregator->aggregation().identifierFilter()[orientation][index].CheckStates.isEmpty()) {
            for (int i=symbol.firstSection(); i<=symbol.lastSection(); ++i) {
                states[i] = mDataAggregator->aggregation().identifierFilter()[orientation][index].Checked;
            }
        } else {
            states.insert(mDataAggregator->aggregation().identifierFilter()[orientation][index].CheckStates);
        }
    }
    return states;
}

bool DataHandler::keepColumn(int column)
{
    if (mDataAggregator->aggregation().viewType() == PredefinedViewEnum::EqnAttributes) {
        return column < PredefinedHeaderLength ? true : false;
    }
    if (mDataAggregator->aggregation().viewType() == PredefinedViewEnum::VarAttributes) {
        return column < PredefinedHeaderLength ? false : true;
    }
    if (mDataAggregator->aggregation().viewType() == PredefinedViewEnum::Jaccobian) {
        return column < PredefinedHeaderLength ? false : true;
    }
    return true;
}

bool DataHandler::keepRow(int row)
{
    if (mDataAggregator->aggregation().viewType() == PredefinedViewEnum::Jaccobian) {
        return row < PredefinedHeaderLength ? false : true;
    }
    return true;
}

}
}
}
