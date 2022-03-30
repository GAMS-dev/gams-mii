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
    AbstractDataAggregator(DataHandler *dataHandler)
        : mDataHandler(dataHandler)
    {
    }

public:
    virtual ~AbstractDataAggregator()
    {
    }

    virtual void aggregateColumns(ModelInstance *modelInstance) = 0;

    virtual void aggregateRows(ModelInstance *modelInstance) = 0;

    DataMatrix& aggregatedDataMatrix()
    {
        return mDataHandler->mAggrMatrix;
    }

    const AggregationMap& aggregationMap() const
    {
        return mDataHandler->mAppliedAggregation.aggregationMap();
    }

    bool useAbsoluteValues() const
    {
        return mDataHandler->mAppliedAggregation.useAbsoluteValues();
    }

    void setFilterForTargetIndexChecked(Qt::Orientation orientation,
                                        int symIndex, int targetIndex)
    {
        mDataHandler->mAppliedAggregation.setLabelState(orientation,
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
    DataHandler *mDataHandler;
};

class IdentityDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    IdentityDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
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
    CountDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                aggregatedDataMatrix()[iter->first] = newRow;
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
                for (int column=0; column<modelInstance->columnCount(); ++column) {
                    QVariant value;
                    Q_FOREACH(auto row, sections) {
                        value = aggregatedValue(value, aggregatedDataMatrix()[row][column]);
                    }
                    if (value.isValid()) newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
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
    MeanDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MeanDataAggregator::aggregatedAbsValue
                                            : &MeanDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                for (int column=0, count = 0; column<modelInstance->columnCount(); ++column, count = 0) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedDataMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            ++count;
                            value += ret.toDouble();
                        }
                    }
                    if (count) newRow[column] = value / count;
                    else newRow[column] = specialValue;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class MedianDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MedianDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MedianDataAggregator::aggregatedAbsValue
                                            : &MedianDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                for (int column=0; column<modelInstance->columnCount(); ++column) {
                    QVariant value = NA;
                    vector<double> values;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedDataMatrix()[row][column], value);
                        if (ret.isValid())
                            values.push_back(ret.toDouble());
                    }
                    if (value.isValid()) newRow[column] = value;
                    else newRow[column] = median(values);
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
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
    MinDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MinDataAggregator::aggregatedAbsValue
                                            : &MinDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                aggregatedDataMatrix()[iter->first] = newRow;
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
                for (int column=0; column<modelInstance->columnCount(); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedDataMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMin(value, ret.toDouble());
                        }
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class MaxDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    MaxDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &MaxDataAggregator::aggregatedAbsValue
                                            : &MaxDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                aggregatedDataMatrix()[iter->first] = newRow;
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
                for (int column=0; column<modelInstance->columnCount(); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedDataMatrix()[row][column], specialValue);
                        if (ret.isValid()) {
                            if (value == 0.0) value = ret.toDouble();
                            else value = qMax(value, ret.toDouble());
                        }
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
            }
        }
    }
};

class SumDataAggregator : public DataHandler::AbstractDataAggregator
{
public:
    SumDataAggregator(DataHandler *dataHandler)
        : DataHandler::AbstractDataAggregator(dataHandler)
    {

    }

    virtual void aggregateColumns(ModelInstance *modelInstance) override
    {
        auto getValue = useAbsoluteValues() ? &SumDataAggregator::aggregatedAbsValue
                                            : &SumDataAggregator::aggregatedValue;
        Q_FOREACH(auto item, aggregationMap()[Qt::Horizontal]) {
            auto var = modelInstance->variable(item.symbolIndex());
            for (auto iter=aggregatedDataMatrix().keyValueBegin();
                 iter!=aggregatedDataMatrix().keyValueEnd(); ++iter) {
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
                aggregatedDataMatrix()[iter->first] = newRow;
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
                for (int column=0; column<modelInstance->columnCount(); ++column) {
                    double value = 0.0;
                    QVariant specialValue = NA;
                    Q_FOREACH(auto row, sections) {
                        auto ret = std::invoke(getValue, this, aggregatedDataMatrix()[row][column], specialValue);
                        if (ret.isValid()) value += ret.toDouble();
                    }
                    if (specialValue.isValid()) newRow[column] = specialValue;
                    else newRow[column] = value;
                }
                Q_FOREACH(auto row, sections) {
                    aggregatedDataMatrix().remove(row);
                }
                setFilterForTargetIndexChecked(Qt::Vertical, eqn.firstSection(), targetRow);
                aggregatedDataMatrix()[targetRow++] = newRow;
            }
        }
    }
};

DataHandler::DataHandler()
    : mDataAggregator(QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this)))
{
}

const Aggregation& DataHandler::appliedAggregation() const
{
    return mAppliedAggregation;
}

void DataHandler::setAppliedAggregation(const Aggregation &aggregation)
{
    mLogicalSectionMapping.clear();
    mAppliedAggregation = aggregation;
    setDataAggregator(mAppliedAggregation.type());
}

void DataHandler::aggregate(ModelInstance *modelInstance)
{
    applyValueFilter();
    mDataAggregator->aggregateRows(modelInstance);
    applyRowFilter(modelInstance);
    setDefaultColumnValues(modelInstance->columnCount());
    mDataAggregator->aggregateColumns(modelInstance);
    applyColumnFilter(modelInstance);
}

QVariant DataHandler::data(int row, int column) const
{// TODO check if IdentityModel stuff is needed
    return isAggregationActive() ? mAggrMatrix[row][column] : mDataMatrix[row][column];
}

int DataHandler::headerData(int logicalIndex, Qt::Orientation orientation) const
{
    if (logicalIndex < mLogicalSectionMapping[orientation].size())
        return mLogicalSectionMapping[orientation][logicalIndex];
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
        for (int c=0; c<modelInstance->columnCount()-PredefinedHeaderLength; ++c) {
            mDataMatrix[r][c+PredefinedHeaderLength] = modelInstance->horizontalAttribute(header, c);
        }
    }
    for (int c=0; c<PredefinedHeaderLength; ++c) {
        auto header = PredefinedHeader.at(c).toLower();
        for (int r=0; r<modelInstance->rowCount()-PredefinedHeaderLength; ++r) {
            mDataMatrix[r+PredefinedHeaderLength][c] = modelInstance->verticalAttribute(header, r);
        }
    }
}

bool DataHandler::isAggregationActive() const
{
    return !mAppliedAggregation.aggregationMap()[Qt::Horizontal].isEmpty() ||
            !mAppliedAggregation.aggregationMap()[Qt::Vertical].isEmpty();
}

void DataHandler::setDataAggregator(Aggregation::Type type)
{
    mDataAggregator = QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this));
    switch (type) {
    case Aggregation::Count:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new CountDataAggregator(this));
        break;
    case Aggregation::Mean:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MeanDataAggregator(this));
        break;
    case Aggregation::Median:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MedianDataAggregator(this));
        break;
    case Aggregation::Maximum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MaxDataAggregator(this));
        break;
    case Aggregation::Minimum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new MinDataAggregator(this));
        break;
    case Aggregation::Sum:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new SumDataAggregator(this));
        break;
    default:
        mDataAggregator = QSharedPointer<AbstractDataAggregator>(new IdentityDataAggregator(this));
        break;
    }
}

void DataHandler::applyValueFilter()
{
    mAggrMatrix.clear();
    for (auto rIter=mDataMatrix.keyValueBegin(); rIter!=mDataMatrix.keyValueEnd(); ++rIter) {
        DataRow newRow;
        Q_FOREACH(auto column, rIter->second.keys()) {
            if (mAppliedAggregation.valueFilter().accepts(rIter->second[column]))
                newRow[column] = rIter->second[column];
        }
        mAggrMatrix[rIter->first] = newRow;
    }
}

void DataHandler::applyRowFilter(ModelInstance *modelInstance)
{
    int row = 0;
    DataMatrix tmpMatrix;
    auto checkState = checkStates(Qt::Vertical, modelInstance);
    Q_FOREACH(auto key, mAggrMatrix.keys()) {
        if (checkState[key] == Qt::Checked) {
            tmpMatrix[row++] = mAggrMatrix[key];
            mLogicalSectionMapping[Qt::Vertical].append(key);
        }
    }
    mAggrMatrix = tmpMatrix;
}

void DataHandler::applyColumnFilter(ModelInstance *modelInstance)
{
    auto checkState = checkStates(Qt::Horizontal, modelInstance);
    for (auto rIter=mAggrMatrix.keyValueBegin(); rIter!=mAggrMatrix.keyValueEnd(); ++rIter) {
        Q_FOREACH(auto column, rIter->second.keys()) {
            if (checkState[column] == Qt::Unchecked)
                rIter->second.remove(column);
        }
        if (mLogicalSectionMapping[Qt::Horizontal].isEmpty()) {
            mLogicalSectionMapping[Qt::Horizontal] = rIter->second.keys();
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
    for (auto rIter=mAggrMatrix.keyValueBegin(); rIter!=mAggrMatrix.keyValueEnd(); ++rIter) {
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
    Q_FOREACH(auto index, mAppliedAggregation.identifierFilter()[orientation].keys()) {
        if (index < PredefinedHeaderLength) {
            states[index] = mAppliedAggregation.identifierFilter()[orientation][index].Checked;
            continue;
        }
        auto symbol = orientation == Qt::Vertical ? modelInstance->equation(index)
                                                  : modelInstance->variable(index);
        if (mAppliedAggregation.identifierFilter()[orientation][index].CheckStates.isEmpty()) {
            for (int i=symbol.firstSection(); i<=symbol.lastSection(); ++i) {
                states[i] = mAppliedAggregation.identifierFilter()[orientation][index].Checked;
            }
        } else {
            states.insert(mAppliedAggregation.identifierFilter()[orientation][index].CheckStates);
        }
    }
    return states;
}

}
}
}
