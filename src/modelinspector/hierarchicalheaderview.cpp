#include "hierarchicalheaderview.h"
#include "filtertreeitem.h"
#include "labelfilterwidget.h"
#include "modelinstance.h"

#include <QPainter>
#include <QPointer>
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>

namespace gams {
namespace studio{
namespace modelinspector {

// TODO (AF) theme stuff (foreground/background color)

typedef QMap<Qt::Orientation, QMap<int, QVector<int>>> IdentifierLabelSections;

class HierarchicalHeaderView::HierarchicalHeaderView_private
{
public:
    HierarchicalHeaderView_private(const HierarchicalHeaderView *headerView)
        : mHeaderView(headerView)
    {
    }

    IdentifierState& identifierState(int symbolIndex, Qt::Orientation orientation)
    {
        return mIdentifierFilter[orientation][symbolIndex];
    }

    void setIdentifierState(const IdentifierState &state, Qt::Orientation orientation)
    {
        mIdentifierFilter[orientation][state.SymbolIndex] = state;
    }

    void setAppliedAggregation(const AggregationSymbols &appliedAggregation)
    {
        mAppliedAggregation = appliedAggregation;
    }

    void resetSymbolLabelFilters()
    {
        mIdentifierFilter.clear();
        mVisibleLabelSections.clear();
    }

    void init()
    {
        QFontMetrics fm(mHeaderView->font());
        mMinHorizontalCellSize = fm.size(0, mHeaderView->mModelInstance->longestVarText());
        mMinVerticalCellSize = fm.size(0, mHeaderView->mModelInstance->longestEqnText());
        mEmptyTextSize = fm.size(0, "");
        mFilterIconSize = QSize(mEmptyTextSize.height(), mEmptyTextSize.height());

        QIcon iconOn(FilterIconOn);
        mPixmapFilterOn = iconOn.pixmap(mFilterIconSize);
        QIcon iconOff(FilterIconOff);
        mPixmapFilterOff = iconOff.pixmap(mFilterIconSize);
    }

    bool isDimensionFilter(const QPoint &position)
    {
        bool ok;
        int logicalIndex = mHeaderView->logicalIndexAt(position);
        int sectionIndex = mHeaderView->model()->headerData(logicalIndex,
                                                            mHeaderView->orientation()).toInt(&ok);
        if (sectionIndex < PredefinedHeaderLength)
            return false;
        QPoint pos;
        if (mHeaderView->orientation() == Qt::Horizontal) {
            pos = QPoint(mHeaderView->sectionViewportPosition(logicalIndex), 0);
        } else {
            pos = QPoint(0, mHeaderView->sectionViewportPosition(logicalIndex));
        }
        return QRect(pos, mFilterIconSize).contains(position);
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
        return symbol.dimension();
    }

    void paintHorizontalSection(QPainter *painter,
                                const QRect &rect,
                                const QStyleOptionHeader &option,
                                int logicalIndex,
                                int sectionIndex)
    {
        int currentTop = rect.y();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QStyleOptionHeader styleOption(option);
        if (styleOption.state.testFlag(QStyle::State_Sunken) ||
                styleOption.state.testFlag(QStyle::State_On)) {
            QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
            styleOption.state &= (~state);
        }

        paintHorizontalCell(painter, rect, styleOption, currentTop,
                            mHeaderView->mModelInstance->headerData(sectionIndex, -1, mHeaderView->orientation()),
                            logicalIndex, sectionIndex, -1, true);
        for (int d=0; d<sectionDimension(sectionIndex); ++d) {
            paintHorizontalCell(painter, rect, styleOption, currentTop,
                                mHeaderView->mModelInstance->headerData(sectionIndex, d, mHeaderView->orientation()),
                                logicalIndex, sectionIndex, d, false);
        }
        paintHorizontalCellSpacing(painter, rect, styleOption, currentTop, sectionIndex);

        painter->setBrushOrigin(oldBrushOrigin);
    }

    void paintHorizontalCell(QPainter *painter,
                             const QRect &rect,
                             const QStyleOptionHeader &option,
                             int &currentTop,
                             const QString &text,
                             int logicalIndex,
                             int sectionIndex,
                             int dimension,
                             bool isSymbol)
    {
        QStyleOptionHeader styleOption(option);
        QSize size = cellSize(styleOption);
        styleOption.rect = QRect(rect.x(), currentTop, rect.width(), size.height());
        auto currentVar = mHeaderView->modelInstance()->variable(sectionIndex);
        styleOption.text = cellText(logicalIndex, sectionIndex, dimension, isSymbol, currentVar, text);

        painter->save();
        if (isSymbol && sectionIndex >= PredefinedHeaderLength) {
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
            styleOption.rect = QRect(rect.x()+mFilterIconSize.width(), currentTop, rect.width(), size.height());
            mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
            if (!styleOption.text.isEmpty() &&
                    mHeaderView->mModelInstance->variable(sectionIndex).isScalar()) {
                painter->drawPixmap(rect.x(), rect.y(), mFilterIconSize.width(),
                                    mFilterIconSize.height(), mPixmapFilterOff);
            } else if (!styleOption.text.isEmpty()){
                painter->drawPixmap(rect.x(), rect.y(), mFilterIconSize.width(),
                                    mFilterIconSize.height(),
                                    mHeaderView->aggregationActive() ? mPixmapFilterOff : mPixmapFilterOn);
            }
        } else {
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
            mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
        }
        painter->restore();

        currentTop += size.height();
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
                              int logicalIndex,
                              int sectionIndex)
    {
        int currentLeft = rect.x();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QStyleOptionHeader styleOption(option);
        if (styleOption.state.testFlag(QStyle::State_Sunken) ||
                styleOption.state.testFlag(QStyle::State_On)) {
            QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
            styleOption.state &= (~state);
        }

        paintVerticalCell(painter, rect, styleOption, currentLeft,
                          mHeaderView->mModelInstance->headerData(sectionIndex, -1, mHeaderView->orientation()),
                          logicalIndex, sectionIndex, -1, true);
        for (int d=0; d<sectionDimension(sectionIndex); ++d) {
            paintVerticalCell(painter, rect, styleOption, currentLeft,
                              mHeaderView->mModelInstance->headerData(sectionIndex, d, mHeaderView->orientation()),
                              logicalIndex, sectionIndex, d, false);
        }
        paintVerticalCellSpacing(painter, rect, styleOption, currentLeft, sectionIndex);

        painter->setBrushOrigin(oldBrushOrigin);
    }

    void paintVerticalCell(QPainter *painter,
                           const QRect &rect,
                           const QStyleOptionHeader &option,
                           int &currentLeft,
                           const QString &text,
                           int logicalIndex,
                           int sectionIndex,
                           int dimension,
                           bool isSymbol)
    {
        QStyleOptionHeader styleOption(option);
        QSize size = cellSize(styleOption);
        styleOption.rect = QRect(currentLeft, rect.y(), size.width(), rect.height());
        auto currentEqn = mHeaderView->modelInstance()->equation(sectionIndex);
        styleOption.text = cellText(logicalIndex, sectionIndex, dimension, isSymbol, currentEqn, text);

        painter->save();
        if (isSymbol && sectionIndex >= PredefinedHeaderLength) {
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
            styleOption.rect = QRect(currentLeft+mFilterIconSize.width(), rect.y(), size.width(), rect.height());
            mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
            if (!styleOption.text.isEmpty() &&
                    mHeaderView->mModelInstance->equation(sectionIndex).isScalar()) {
                painter->drawPixmap(currentLeft, rect.y(), mFilterIconSize.width(),
                                    mFilterIconSize.height(), mPixmapFilterOff);
            } else if (!styleOption.text.isEmpty()) {
                painter->drawPixmap(rect.x(), rect.y(), mFilterIconSize.width(),
                                    mFilterIconSize.height(),
                                    mHeaderView->aggregationActive() ? mPixmapFilterOff : mPixmapFilterOn);
            }
        } else {
            mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &styleOption, painter, mHeaderView);
            mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &styleOption, painter, mHeaderView);
        }
        painter->restore();

        currentLeft += size.width();
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

    QString cellText(int logicalIndex, int sectionIndex, int dimension,
                     bool isSymbol, const SymbolInfo &symbol,
                     const QString &text)
    {
        if (dimension >= 0 && mAppliedAggregation.contains(symbol.firstSection())) {
            return mAppliedAggregation[symbol.firstSection()].label(sectionIndex, dimension);
        }
        if (isSymbol && logicalIndex > 0) {
            int prevSectionIdx = this->sectionIndex(logicalIndex-1);
            if (symbol.contains(prevSectionIdx))
                return QString();
        } else if (logicalIndex > 0) {
            int prevSectionIdx = this->sectionIndex(logicalIndex-1);
            if (symbol.contains(prevSectionIdx) &&
                    symbol.label(sectionIndex, dimension) == symbol.label(prevSectionIdx, dimension) &&
                        (dimension == 0 ||
                            symbol.label(sectionIndex, dimension-1) == symbol.label(prevSectionIdx, dimension-1))) {
                    return QString();
            }
        }
        return text;
    }

    QSize cellSize(QStyleOptionHeader styleOption) const
    {
        QSize size = mHeaderView->orientation() == Qt::Vertical ? mMinVerticalCellSize :
                                                                  mMinHorizontalCellSize;

        QSize decorationsSize(mHeaderView->style()->sizeFromContents(QStyle::CT_HeaderSection,
                                                                     &styleOption,
                                                                     QSize(),
                                                                     mHeaderView));
        QSize iconWidth(mFilterIconSize.width(), 0.0);
        return size + decorationsSize + iconWidth - mEmptyTextSize;
    }

    FilterTreeItem* filterTree(int logicalIndex, int sectionIdx, const SymbolInfo &symbol)
    {
        auto root = new FilterTreeItem;
        root->setCheckable(false);
        root->setSymbolIndex(symbol.firstSection());
        root->setSectionIndex(symbol.firstSection());

        QVector<int> visibleSections = visibleLabelSections(logicalIndex, sectionIdx, symbol);

        QString label;
        QMap<int, QStringList> subData;
        auto filter = identifierState(symbol.firstSection(), mHeaderView->orientation());
        for (int section=symbol.firstSection(); section<=symbol.lastSection(); ++section) {
            if (symbol.sectionLabels().isEmpty()) {
                continue;
            }
            auto subLabels = symbol.sectionLabels()[section];
            if (symbol.sectionLabels()[section].isEmpty()) {
                continue;
            }
            if (label.isEmpty()) {
                label = subLabels.takeFirst();
                subData[section] = subLabels;
            } else if (label == symbol.sectionLabels()[section].first()) {
                subLabels.removeFirst();
                subData[section] = subLabels;
            } else {
                labelFilterTree(label, subData, filter, visibleSections, symbol.firstSection(), root);
                label = subLabels.takeFirst();
                subData.clear();
                subData[section] = subLabels;
            }
        }
        labelFilterTree(label, subData, filter, visibleSections, symbol.firstSection(), root);
        return root;
    }

    int maxSymbolDimension(Qt::Orientation orientation)
    {
        if (orientation == Qt::Horizontal &&
                mHeaderView->model()->columnCount() != mHeaderView->modelInstance()->columnCount()) {
            int dimension = 0;
            for (int c=0; c<mHeaderView->model()->columnCount(); ++c) {
                int sectionIdx = sectionIndex(c);
                dimension = qMax(dimension, mHeaderView->modelInstance()->variable(sectionIdx).dimension());
            }
            return  dimension;
        } else if (orientation == Qt::Vertical &&
                   mHeaderView->model()->rowCount() != mHeaderView->modelInstance()->rowCount()) {
            int dimension = 0;
            for (int c=0; c<mHeaderView->model()->rowCount(); ++c) {
                int sectionIdx = sectionIndex(c);
                dimension = qMax(dimension, mHeaderView->modelInstance()->equation(sectionIdx).dimension());
            }
            return dimension;
        }
        return orientation == Qt::Horizontal ? mHeaderView->modelInstance()->maxVariableDimension() :
                                               mHeaderView->modelInstance()->maxEquationDimension();
    }

private:
    QVector<int> visibleLabelSections(int logicalIndex, int sectionIdx, const SymbolInfo &symbol)
    {
        if (mVisibleLabelSections[mHeaderView->orientation()].contains(symbol.firstSection())) {
            return mVisibleLabelSections[mHeaderView->orientation()][symbol.firstSection()];
        }
        QVector<int> visibleSections;
        int logicalEnd = logicalIndex + symbol.lastSection() - sectionIdx;
        int logicalStart = logicalEnd - symbol.entries() + 1;
        for (int i=logicalStart; i<=logicalEnd; ++i) {
            int sIndex = sectionIndex(i);
            if (symbol.contains(sIndex))
                visibleSections.push_back(sIndex);
        }
        mVisibleLabelSections[mHeaderView->orientation()][symbol.firstSection()] = visibleSections;
        return mVisibleLabelSections[mHeaderView->orientation()][symbol.firstSection()];
    }

    void labelFilterTree(const QString label,
                         const QMap<int, QStringList> &data,
                         const IdentifierState &state,
                         QVector<int> visibleSections,
                         int symbolIndex,
                         FilterTreeItem *parent)
    {
        if (data.isEmpty() || label.isEmpty()) return;
        FilterTreeItem *labelItem;
        bool enabled = visibleSections.contains(data.firstKey());
        if (state.isValid() && state.CheckStates.contains(data.firstKey())) {
            labelItem = new FilterTreeItem(label,
                                           enabled ? state.CheckStates[data.firstKey()] : Qt::Unchecked,
                                           parent);
            labelItem->setSectionIndex(data.firstKey());
            labelItem->setEnabled(enabled);
        } else {
            labelItem = new FilterTreeItem(label, enabled ? Qt::Checked : Qt::Unchecked, parent);
            labelItem->setSectionIndex(data.firstKey());
            labelItem->setEnabled(enabled);
        }
        labelItem->setSymbolIndex(symbolIndex);
        parent->append(labelItem);

        QString subLabel;
        QMap<int, QStringList> subData;
        for (auto iter=data.keyValueBegin(); iter!=data.keyValueEnd(); ++iter) {
            auto subLabels = iter->second;
            if (subLabels.isEmpty()) {
                continue;
            }
            if (subLabel.isEmpty()) {
                subLabel = subLabels.takeFirst();
                subData[iter->first] = subLabels;
            } else if (subLabel == subLabels.first()) {
                subLabels.removeFirst();
                subData[iter->first] = subLabels;
            } else {
                labelFilterTree(subLabel, subData, state, visibleSections, symbolIndex, labelItem);
                subLabel = subLabels.takeFirst();
                subData.clear();
                subData[iter->first] = subLabels;
            }
        }
        labelFilterTree(subLabel, subData, state, visibleSections, symbolIndex, labelItem);
    }

private:
    const QString FilterIconOn = ":/img/filter";
    const QString FilterIconOff = ":/img/filter-off";

    const HierarchicalHeaderView *mHeaderView;

    QSize mFilterIconSize;
    QSize mMinHorizontalCellSize;
    QSize mMinVerticalCellSize;
    QSize mEmptyTextSize;

    QPixmap mPixmapFilterOn;
    QPixmap mPixmapFilterOff;

    IdentifierFilter mIdentifierFilter;
    IdentifierLabelSections mVisibleLabelSections;

    AggregationSymbols mAppliedAggregation;
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
            this, &HierarchicalHeaderView::on_filterChanged);
}

HierarchicalHeaderView::~HierarchicalHeaderView()
{
    delete mPrivate;
}

QSharedPointer<ModelInstance> HierarchicalHeaderView::modelInstance() const
{
    return mModelInstance;
}

void HierarchicalHeaderView::setAppliedAggregation(const AggregationSymbols &appliedAggregation)
{
    mPrivate->setAppliedAggregation(appliedAggregation);
}

void HierarchicalHeaderView::setModel(QAbstractItemModel *model)
{
    mPrivate->init();
    QHeaderView::setModel(model);
}

void HierarchicalHeaderView::customMenuRequested(QPoint position)
{
    if (aggregationActive())
        return;

    bool ok;
    int logicalIndex = logicalIndexAt(position);
    int sectionIndex = model()->headerData(logicalIndex, orientation()).toInt(&ok);
    if (!ok) return;

    SymbolInfo symbol;
    if (orientation() == Qt::Horizontal) {
        symbol = mModelInstance->variable(sectionIndex);
    } else {
        symbol = mModelInstance->equation(sectionIndex);
    }
    if (symbol.isScalar())
        return;

    auto filterTree = mPrivate->filterTree(logicalIndex, sectionIndex, symbol);
    mFilterWidget->setData(filterTree);
    mFilterMenu->popup(viewport()->mapToGlobal(position));
}

void HierarchicalHeaderView::resetSymbolLabelFilters()
{
    mPrivate->resetSymbolLabelFilters();
}

void HierarchicalHeaderView::on_filterChanged(const IdentifierState& state,
                                              Qt::Orientation orientation)
{
    mPrivate->setIdentifierState(state, orientation);
    emit filterChanged(state, orientation);
}

void HierarchicalHeaderView::paintSection(QPainter *painter,
                                          const QRect &rect,
                                          int logicalIndex) const
{
    if (rect.isValid()) {
        auto option = styleOptionForCell(logicalIndex);
        int sectionIdx = mPrivate->sectionIndex(logicalIndex);
        if (orientation() == Qt::Horizontal) {
            mPrivate->paintHorizontalSection(painter, rect, option, logicalIndex, sectionIdx);
        } else {
            mPrivate->paintVerticalSection(painter, rect, option, logicalIndex, sectionIdx);
        }
        return;
    }
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize HierarchicalHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    QStyleOptionHeader styleOption(styleOptionForCell(logicalIndex));
    QSize size(mPrivate->cellSize(styleOption));

    int dimension = mPrivate->maxSymbolDimension(orientation());
    for (int d=0; d<dimension; ++d) {
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
        event->accept();
        return;
    }
    QHeaderView::mousePressEvent(event);
}

bool HierarchicalHeaderView::aggregationActive() const
{
    return mModelInstance->aggregationActive();
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
