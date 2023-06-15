#ifndef ABSTRACTTABLEVIEWFRAME_H
#define ABSTRACTTABLEVIEWFRAME_H

#include "common.h"
#include "abstractviewframe.h"

#include <QSharedPointer>

class QAbstractItemModel;

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class StandardTableViewFrame;
}

class AbstractTableViewFrame : public AbstractViewFrame
{
    Q_OBJECT

public:
    AbstractTableViewFrame(QWidget *parent = nullptr,
                           Qt::WindowFlags f = Qt::WindowFlags());

    virtual ~AbstractTableViewFrame();

    void setSearchSelection(const SearchResult::SearchEntry &result) override;

    SearchResult& search(const QString &term, bool isRegEx) override;

    void zoomIn() override;

    void zoomOut() override;

    void resetZoom() override;

signals:
    void filtersChanged();

    void searchResultUpdated(const QList<SearchResult>&);

protected slots:
    virtual void setIdentifierLabelFilter(const gams::studio::modelinspector::IdentifierState &state,
                                          Qt::Orientation orientation);

protected:
    Ui::StandardTableViewFrame* ui;
};

}
}
}

#endif // ABSTRACTTABLEVIEWFRAME_H
