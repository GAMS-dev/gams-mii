#ifndef DATAMATRIX_H
#define DATAMATRIX_H

#include <QVariant>

namespace gams {
namespace studio {
namespace mii {

class DataRow
{
public:
    DataRow();

    DataRow(int entries);

    DataRow(const DataRow& other);

    DataRow(DataRow&& other) noexcept;

    ~DataRow();

    int entries() const;

    void setEntries(int entries);

    int entriesNl() const;

    void setEntriesNl(int entries);

    int* colIdx() const;

    void setColIdx(int* colIdx);

    ///
    /// \brief Pointer to input data, always available.
    /// \return Pointer to input data.
    ///
    double* inputData() const;

    void setInputData(double* inputData);

    ///
    /// \brief Pointer to output data, if available.
    /// \return Pointer to ouput data.
    ///
    double* outputData() const;

    void setOutputData(double* outputData);

    int* nlFlags() const;

    void setNlFlags(int *nlFlags);

    QVariant inputValue(int index, int lastSymIndex);

    QVariant outputValue(int index, int lastSymIndex);

    DataRow& operator=(const DataRow& other);

    DataRow& operator=(DataRow&& other) noexcept;

private:
    int mEntries;
    int mEntriesNl;
    int* mColIdx;
    double *mInputData;
    double *mOutputData;
    int *mNlFlags;
};

class DataMatrix
{
public:
    DataMatrix();

    DataMatrix(int rows, int columns, int modelType);

    DataMatrix(const DataMatrix& other);

    DataMatrix(DataMatrix&& other) noexcept;

    ~DataMatrix();

    int rowCount() const;

    int columnCount() const;

    double* evalPoint();

    DataRow* row(int row);

    bool isLinear() const;

    DataMatrix& operator=(const DataMatrix& other);

    DataMatrix& operator=(DataMatrix&& other) noexcept;

private:
    int mRowCount;
    int mColumnCount;
    DataRow *mRows;
    double *mEvalPoint;
    int mModelType;
};

}
}
}

#endif // DATAMATRIX_H
