#include "rangechart.h"

#include <QPieSeries>

namespace gams {
namespace studio {
namespace modelinspector {

RangeChart::RangeChart(QWidget *parent)
    : QChartView(parent)
    , mSeries(new QPieSeries)
{
    chart()->addSeries(mSeries);
    chart()->setTitle("Equation Counts");
    chart()->legend()->setAlignment(Qt::AlignBottom);
    setRenderHint(QPainter::Antialiasing);
}

void RangeChart::setData(const QMap<EquationType, double> &data)
{
    if (data.isEmpty())
            return;
    for (auto key: data.keys()) {
        if (data.value(key) > 0)
            appendSlice(key, data.value(key));
    }
}

void RangeChart::appendSlice(EquationType type, double value)
{
    switch (type) {
    case EquationType::E:
        mSeries->append("=E=", value);
        break;
    case EquationType::G:
        mSeries->append("=G=", value);
        break;
    case EquationType::L:
        mSeries->append("=L=", value);
        break;
    case EquationType::N:
        mSeries->append("=N=", value);
        break;
    case EquationType::X:
        mSeries->append("=X=", value);
        break;
    case EquationType::C:
        mSeries->append("=C=", value);
        break;
    case EquationType::B:
        mSeries->append("=B=", value);
        break;
    }
}

VariableChart::VariableChart(QWidget *parent)
    : QChartView(parent)
    , mSeries(new QPieSeries)
{
    chart()->addSeries(mSeries);
    chart()->setTitle("Variable Counts");
    //chart()->legend()->setAlignment(Qt::AlignBottom);
    chart()->legend()->setVisible(false);
    setRenderHint(QPainter::Antialiasing);
}

void VariableChart::setData(const QMap<VariableType, double> &data)
{
    if (data.isEmpty())
            return;
    for (auto key: data.keys()) {
        if (data.value(key) > 0)
            appendSlice(key, data.value(key));
    }
}

void VariableChart::appendSlice(VariableType type, double value)
{// TODO qreal?
    auto slice = new QPieSlice;
    slice->setLabelVisible();
    slice->setValue(value);
    switch (type) {
    case VariableType::Continous:
        slice->setLabel("Continous");
        break;
    case VariableType::Binary:
        slice->setLabel("Binary");
        break;
    case VariableType::Integer:
        slice->setLabel("Integer");
        break;
    case VariableType::SOS1:
        slice->setLabel("SOS1");
        break;
    case VariableType::SOS2:
        slice->setLabel("SOS2");
        break;
    case VariableType::SCont:
        slice->setLabel("SCont");
        break;
    case VariableType::SInt:
        slice->setLabel("SInt");
        break;
    }
    mSeries->append(slice);
}

CoefficientChart::CoefficientChart(QWidget *parent)
    : QChartView(parent)
    , mSeries(new QPieSeries)
{
    chart()->addSeries(mSeries);
    chart()->setTitle("Coefficient Counts");
    //chart()->legend()->setAlignment(Qt::AlignBottom);
    chart()->legend()->setVisible(false);
    setRenderHint(QPainter::Antialiasing);
}

void CoefficientChart::setData(const QMap<CoefficientType, double> &data)
{
    if (data.isEmpty())
            return;
    for (auto key: data.keys()) {
        if (data.value(key) > 0)
            appendSlice(key, data.value(key));
    }
}

void CoefficientChart::appendSlice(CoefficientType type, double value)
{// TODO qreal?
    auto slice = new QPieSlice;
    slice->setLabelVisible();
    slice->setValue(value);
    switch (type) {
    case CoefficientType::Positive:
        slice->setLabel("Positive");
        break;
    case CoefficientType::Negative:
        slice->setLabel("Negative");
        break;
    case CoefficientType::NonLinear:
        slice->setLabel("NonLinear");
        break;
    }
    mSeries->append(slice);
}

DummyChart::DummyChart(QWidget *parent)
    : QChartView(parent)
    , mSeries(new QPieSeries)
{
    chart()->addSeries(mSeries);
    chart()->setTitle("Dummy");
}

}
}
}
