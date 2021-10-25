#include "hierarchicalheaderview.h"
#include "filtertreeitem.h"
#include "labelfilterwidget.h"
#include "modelinstance.h"

#include <QPainter>
#include <QPointer>
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

// TODO (AF) theme stuff (foreground/background color)

class HierarchicalHeaderView::HierarchicalHeaderView_private
{
public:
    HierarchicalHeaderView_private(const HierarchicalHeaderView *headerView)
        : mHeaderView(headerView)
    {
    }

    void init() {
        QFontMetrics fm(mHeaderView->font());
        mMinHorizontalCellSize = fm.size(0, mHeaderView->mModelInstance->longestVarText());
        mMinVerticalCellSize = fm.size(0, mHeaderView->mModelInstance->longestEqnText());
        mEmptyTextSize = fm.size(0, "");
        mFilterIconSize = QSize(mEmptyTextSize.height(), mEmptyTextSize.height());

        QIcon icon(FilterIconOn);
        mPixmapFilterOn = icon.pixmap(mFilterIconSize);
    }

    bool isDimensionFilter(const QPoint &pos) {
        Q_FOREACH(const auto &rect, mDimensionFilterSpots) {
            if (rect.contains(pos))
                return true;
        }
        return false;
    }

    int sectionIndex(int logicalIndex) const
    {
        bool ok;
        auto index = mHeaderView->model()->headerData(logicalIndex,
                                                      mHeaderView->orientation()).toInt(&ok);
        if (!ok) index = -1;
        return index;
    }

    int sectionDimension(int sectionIndex)
    {
        if (sectionIndex < PredefinedHeaderLength)
            return 0;
        SymbolInfo symbol;
        if (mHeaderView->orientation() == Qt::Horizontal) {
            symbol = mHeaderView->mModelInstance->variable(sectionIndex);
        } else {
            symbol = mHeaderView->mModelInstance->equation(sectionIndex);
        }
        return symbol.Dimension;
    }

    void paintHorizontalSection(QPainter *painter,
                                const QRect &rect,
                                const QStyleOptionHeader &option,
                                int sectionIndex)
    {
        int currentTop = rect.y();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QStyleOptionHeader styleOption(option);
        if (styleOption.state.testFlag(QStyle::State_Sunken) ||
                styleOption.state.testFlag(QStyle::State_On)) {
            // TODO mind sparse cells
            QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
            styleOption.state &= (~state);
        }

        paintHorizontalCell(painter, rect, styleOption, currentTop,
                            mHeaderView->mModelInstance->headerData(sectionIndex, -1, mHeaderView->orientation()));
        for (int d=0; d<sectionDimension(sectionIndex); ++d) {
            paintHorizontalCell(painter, rect, styleOption, currentTop,
                                mHeaderView->mModelInstance->headerData(sectionIndex, d, mHeaderView->orientation()));
        }
        paintHorizontalCellSpacing(painter, rect, styleOption, currentTop, sectionIndex);

        painter->setBrushOrigin(oldBrushOrigin);
    }

    void paintHorizontalCell(QPainter *painter,
                             const QRect &rect,
                             const QStyleOptionHeader &option,
                             int &currentTop,
                             const QString &text)
    {
        QStyleOptionHeader styleOption(option);
        QSize size = cellSize(styleOption);
        styleOption.text = text;
        styleOption.rect = QRect(rect.x(), currentTop, rect.width(), size.height());
        currentTop += size.height();

        painter->save();
        mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
        mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
        painter->restore();
    }

    void paintHorizontalCellSpacing(QPainter *painter,
                                    const QRect &rect,
                                    const QStyleOptionHeader &option,
                                    int &currentTop,
                                    int sectionIndex)
    {
        QStyleOptionHeader styleOption(option);
        painter->save();
        if (sectionDimension(sectionIndex) < mHeaderView->mModelInstance->maxVariableDimension()) {
            styleOption.rect = QRect(rect.x(), currentTop, rect.width(), rect.height());
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
        }
        painter->restore();
    }

    void paintVerticalSection(QPainter *painter,
                              const QRect &rect,
                              QStyleOptionHeader option,
                              int sectionIndex)
    {
        int currentLeft = rect.x();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QStyleOptionHeader styleOption(option);
        if (styleOption.state.testFlag(QStyle::State_Sunken) ||
                styleOption.state.testFlag(QStyle::State_On)) {
            // TODO mind sparse cells
            QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
            styleOption.state &= (~state);
        }

        paintVerticalCell(painter, rect, styleOption, currentLeft,
                          mHeaderView->mModelInstance->headerData(sectionIndex, -1, mHeaderView->orientation()));
        for (int d=0; d<sectionDimension(sectionIndex); ++d) {
            paintVerticalCell(painter, rect, styleOption, currentLeft,
                              mHeaderView->mModelInstance->headerData(sectionIndex, d, mHeaderView->orientation()));
        }
        paintVerticalCellSpacing(painter, rect, styleOption, currentLeft, sectionIndex);

        painter->setBrushOrigin(oldBrushOrigin);
    }

    void paintVerticalCell(QPainter *painter,
                           const QRect &rect,
                           const QStyleOptionHeader &option,
                           int &currentLeft,
                           const QString &text)
    {
        QStyleOptionHeader styleOption(option);
        QSize size = cellSize(styleOption);
        styleOption.text = text;
        styleOption.rect = QRect(currentLeft, rect.y(), size.width(), rect.height());
        currentLeft += size.width();

        painter->save();
        mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
        mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
        painter->restore();
    }

    void paintVerticalCellSpacing(QPainter *painter,
                                  const QRect &rect,
                                  const QStyleOptionHeader &option,
                                  int &currentLeft,
                                  int sectionIndex)
    {
        QStyleOptionHeader styleOption(option);
        painter->save();
        if (sectionDimension(sectionIndex) < mHeaderView->mModelInstance->maxEquationDimension()) {
            styleOption.rect = QRect(currentLeft, rect.y(), rect.width(), rect.height());
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
        }
        painter->restore();
    }

    QSize cellSize(QStyleOptionHeader styleOption) const
    {
        QSize size = mHeaderView->orientation() == Qt::Vertical ? mMinVerticalCellSize :
                                                                  mMinHorizontalCellSize;

        QSize decorationsSize(mHeaderView->style()->sizeFromContents(QStyle::CT_HeaderSection,
                                                                     &styleOption,
                                                                     QSize(),
                                                                     mHeaderView));
        return size + decorationsSize - mEmptyTextSize;
    }

    inline QSize filterIconSize() const
    {
        return mFilterIconSize;
    }

private:
    const QString FilterIconOn = ":/img/filter";
    const QString FilterIconOff = ":/img/filter-off";

    const HierarchicalHeaderView *mHeaderView;
    QVector<QRect> mDimensionFilterSpots;

    QSize mFilterIconSize;
    QSize mMinHorizontalCellSize;
    QSize mMinVerticalCellSize;
    QSize mEmptyTextSize;

    QPixmap mPixmapFilterOn;
    //QPixmap mFilterPixmapOff; // TODO (AF) off icon
};

HierarchicalHeaderView::HierarchicalHeaderView(Qt::Orientation orientation,
                                               QSharedPointer<ModelInstance> modelInstance,
                                               QWidget *parent)
    : QHeaderView(orientation, parent)
    , mPrivate(new HierarchicalHeaderView_private(this))
    , mModelInstance(modelInstance)
    , mFilterMenu(new QMenu(this))
    , mFilterWidget(new LabelFilterWidget(orientation, this))
{
    setHighlightSections(true);
    setSectionsClickable(true);
    setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);

    auto filterAction = new QWidgetAction(mFilterMenu);
    filterAction->setDefaultWidget(mFilterWidget);
    mFilterMenu->addAction(filterAction);

    connect(this, &HierarchicalHeaderView::customContextMenuRequested,
            this, &HierarchicalHeaderView::customMenuRequested);
    connect(mFilterWidget, &LabelFilterWidget::filterChanged,
            this, &HierarchicalHeaderView::filterChanged);
}

HierarchicalHeaderView::~HierarchicalHeaderView()
{
    delete mPrivate;
}

void HierarchicalHeaderView::setModel(QAbstractItemModel *model)
{
    mPrivate->init();
    QHeaderView::setModel(model);
}

void HierarchicalHeaderView::customMenuRequested(QPoint position)
{// TODO ...
    //QStandardItem *item;
    //if (orientation() == Qt::Horizontal) {
    //    item =  mModelInstance->horizontalItem(logicalIndexAt(position));
    //} else {
    //    item = mModelInstance->verticalItem(logicalIndexAt(position));
    //}
    //if (item) {
    //    TreeGenerator generator(mModelInstance, orientation());
    //    auto root = generator.generate(item);
    //    mFilterWidget->setData(root);
    //    mFilterMenu->popup(viewport()->mapToGlobal(position));
    //}
}

void HierarchicalHeaderView::paintSection(QPainter *painter,
                                          const QRect &rect,
                                          int logicalIndex) const
{
    if (rect.isValid()) {
        auto option = styleOptionForCell(logicalIndex);
        int sectionIdx = mPrivate->sectionIndex(logicalIndex);
        if (orientation() == Qt::Horizontal) {
            mPrivate->paintHorizontalSection(painter, rect, option, sectionIdx);
        } else {
            mPrivate->paintVerticalSection(painter, rect, option, sectionIdx);
        }
        return;
    }
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize HierarchicalHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    QStyleOptionHeader styleOption(styleOptionForCell(logicalIndex));
    QSize size(mPrivate->cellSize(styleOption));

    int sectionIdx = mPrivate->sectionIndex(logicalIndex);
    for (int d=0; d<mPrivate->sectionDimension(sectionIdx); ++d) {
        if (orientation() == Qt::Horizontal) {
            size.rheight() += mPrivate->cellSize(styleOption).height();
        } else {
            size.rwidth() += mPrivate->cellSize(styleOption).width();
        }
    }
    return size;
}

void HierarchicalHeaderView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mPrivate->isDimensionFilter(event->pos())) {
        emit customContextMenuRequested(event->pos());
        return;
    }
    QHeaderView::mousePressEvent(event);
}

QStyleOptionHeader HierarchicalHeaderView::styleOptionForCell(int logicalIndex) const
{
    QStyleOptionHeader option;
    initStyleOption(&option);

    if (window()->isActiveWindow())
        option.state |= QStyle::State_Active;

    option.textAlignment = Qt::AlignLeft | Qt::AlignTop;
    option.iconAlignment = Qt::AlignVCenter;
    option.section = logicalIndex;

    int visual = visualIndex(logicalIndex);
    if (count() == 1) {
        option.position = QStyleOptionHeader::OnlyOneSection;
    } else {
        if (visual == 0)
            option.position = QStyleOptionHeader::Beginning;
        else
            option.position = (visual == count()-1 ? QStyleOptionHeader::End :
                                                     QStyleOptionHeader::Middle);
    }

    if(sectionsClickable()) {
        if(highlightSections() && selectionModel()) {
            if(orientation() == Qt::Horizontal) {
                if(selectionModel()->columnIntersectsSelection(logicalIndex, rootIndex()))
                    option.state |= QStyle::State_On;

                if(selectionModel()->isColumnSelected(logicalIndex, rootIndex()))
                    option.state |= QStyle::State_Sunken;
            } else {
                if(selectionModel()->rowIntersectsSelection(logicalIndex, rootIndex()))
                    option.state |= QStyle::State_On;

                if(selectionModel()->isRowSelected(logicalIndex, rootIndex()))
                    option.state |= QStyle::State_Sunken;
            }
        }
    }

    if(selectionModel()) {
        bool previousSelected = false;

        if(orientation() == Qt::Horizontal)
            previousSelected = selectionModel()->isColumnSelected(this->logicalIndex(visual - 1),
                                                                  rootIndex());
        else
            previousSelected = selectionModel()->isRowSelected(this->logicalIndex(visual - 1),
                                                               rootIndex());

        bool nextSelected = false;

        if(orientation() == Qt::Horizontal)
            nextSelected = selectionModel()->isColumnSelected(this->logicalIndex(visual + 1),
                                                              rootIndex());
        else
            nextSelected = selectionModel()->isRowSelected(this->logicalIndex(visual + 1),
                                                           rootIndex());

        if (previousSelected && nextSelected) {
            option.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
        } else {
            if (previousSelected) {
                option.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
            } else {
                if (nextSelected)
                    option.selectedPosition = QStyleOptionHeader::NextIsSelected;
                else
                    option.selectedPosition = QStyleOptionHeader::NotAdjacent;
            }
        }
    }

    return option;
}

}
}
}
