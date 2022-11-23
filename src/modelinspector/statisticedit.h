#ifndef STATISTICEDIT_H
#define STATISTICEDIT_H

#include <QMap>
#include <QTextEdit>

namespace gams {
namespace studio {
namespace modelinspector {

class AbstractModelInstance;

class StatisticEdit : public QTextEdit
{
    Q_OBJECT

public:
    StatisticEdit(QWidget *parent = nullptr);

    bool event(QEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    void resetZoom();

    void showStatistic(const QSharedPointer<AbstractModelInstance> &modelInstance);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QFont mBaseFont;

    const QMap<QString, QString> mToolTipMapping = {
        {"=e=", "Equality: right-hand side must equal left-hand side"},
        {"=g=", "Greater than: left-hand side must be greater than or equal to right-hand side"},
        {"=l=", "Less than: left-hand side must be less than or equal to right-hand side"},
        {"=n=", "No relationship implied between left-hand side and right-hand side. This equation type is ideally suited for use in MCP models and in variational inequalities."},
        {"=x=", "Equation is defined by external programs"},
        {"=c=", "Conic constraint"},
        {"=b=", "Boolean equation"},
        {"sos1", "Special Order Sets of Type 1"},
        {"sos2", "Special Order Sets of Type 2"},
        {"scont", "Semi-Continous Variables"},
        {"sint", "Semi-Integer Variables"},
    };
};

}
}
}

#endif // STATISTICEDIT_H
