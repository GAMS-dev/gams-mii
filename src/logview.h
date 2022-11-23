#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QTextEdit>

class LogView : public QTextEdit
{
    Q_OBJECT

public:
    LogView(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void resetZoom();

private:
    QFont mBaseFont;
};

#endif // LOGVIEW_H
