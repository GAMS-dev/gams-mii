#ifndef POSTOPTTREEVIEWFRAME_H
#define POSTOPTTREEVIEWFRAME_H

#include "abstractviewframe.h"

namespace gams {
namespace studio{
namespace modelinspector {

namespace Ui {
class PostoptTreeViewFrame;
}


class PostoptTreeViewFrame final : public AbstractViewFrame
{
    Q_OBJECT

public:
    PostoptTreeViewFrame(QWidget *parent = nullptr,
                         Qt::WindowFlags f = Qt::WindowFlags());

    PostoptTreeViewFrame(QSharedPointer<AbstractModelInstance> modelInstance,
                         QSharedPointer<AbstractViewConfiguration> viewConfig,
                         QWidget *parent = nullptr,
                         Qt::WindowFlags f = Qt::WindowFlags());

    ~PostoptTreeViewFrame();

    AbstractViewFrame* clone(int view) override;

    void setIdentifierFilter(const IdentifierFilter &filter) override;

    void setAggregation(const Aggregation &aggregation) override;

    void setShowAbsoluteValues(bool absoluteValues) override;

    SearchResult& search(const QString &term, bool isRegEx) override;

    void setSearchSelection(const SearchResult::SearchEntry &result) override;

    void setupView(QSharedPointer<AbstractModelInstance> modelInstance) override;

    ViewDataType type() const override;

    void reset() override;

    void updateView() override;

    void zoomIn() override;

    void zoomOut() override;

    void resetZoom() override;

private:
    void setupView();

protected:
    Ui::PostoptTreeViewFrame* ui;
};

}
}
}

#endif // POSTOPTTREEVIEWFRAME_H
