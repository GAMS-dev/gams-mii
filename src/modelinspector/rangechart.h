#ifndef RANGECHART_H
#define RANGECHART_H

#include <QChartView>

QT_CHARTS_BEGIN_NAMESPACE
class QPieSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace gams {
namespace studio {
namespace modelinspector {

enum class EquationType {
    E,
    G,
    L,
    N,
    X,
    C,
    B
};

enum class VariableType {
    Continous,
    Binary,
    Integer,
    SOS1,
    SOS2,
    SCont,
    SInt
};

enum class CoefficientType {
    Positive,
    Negative,
    NonLinear
};

class RangeChart : public QChartView
{// TODO rename
    Q_OBJECT

public:
    RangeChart(QWidget *parent = nullptr);

    void setData(const QMap<EquationType, double> &data);

private:
    void appendSlice(EquationType type, double value);

private:
    QPieSeries *mSeries;
};

class VariableChart : public QChartView
{
    Q_OBJECT

public:
    VariableChart(QWidget *parent = nullptr);

    void setData(const QMap<VariableType, double> &data);

private:
    void appendSlice(VariableType type, double value);

private:
    QPieSeries *mSeries;
};

class CoefficientChart : public QChartView
{
    Q_OBJECT

public:
    CoefficientChart(QWidget *parent = nullptr);

    void setData(const QMap<CoefficientType, double> &data);

private:
    void appendSlice(CoefficientType type, double value);

private:
    QPieSeries *mSeries;
};

class DummyChart : public QChartView
{
    Q_OBJECT

public:
    DummyChart(QWidget *parent = nullptr);

private:
    QPieSeries *mSeries;
};

}
}
}

#endif // RANGECHART_H
