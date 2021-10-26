#include "hierarchicalheaderview.h"
#include "filtertreeitem.h"
#include "labelfilterwidget.h"
#include "modelinstance.h"

#include <QPainter>
#include <QPointer>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QWidgetAction>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

// TODO (AF) theme stuff (foreground/background color)

class TreeGenerator
{
public:
    TreeGenerator(QSharedPointer<ModelInstance> modelInstance,
                  Qt::Orientation orientation)
        : mModelInstance(modelInstance)
        , mOrientation(orientation)
    {
    }

    FilterTreeItem* generate(QStandardItem *item)
    {
        auto root = new FilterTreeItem(item->data(Qt::DisplayRole).toString(),
                                       item->checkState(),
                                       mModelInstance->itemToIndex(mOrientation, item));
        appendChildren(root, item);
        return root;
    }

private:

    void appendChildren(FilterTreeItem *parent, QStandardItem *item)
    {
        for (int c=0; c<item->columnCount(); ++c) {
            auto child = item->child(0, c);
            auto fitem = new FilterTreeItem(child->data(Qt::DisplayRole).toString(),
                                            child->checkState(),
                                            mModelInstance->itemToIndex(mOrientation, child),
                                            parent);
            parent->append(fitem);
            appendChildren(fitem, child);
        }
    }

private:
    QSharedPointer<ModelInstance> mModelInstance;
    Qt::Orientation mOrientation;
};


class HierarchicalHeaderView::HierarchicalHeaderView_private
{
public:
    HierarchicalHeaderView_private(const HierarchicalHeaderView *headerView)
        : mHeaderView(headerView)
    {
    }

    void initFromModel(Qt::Orientation orientation, QAbstractItemModel* model)
    {
        mModel = QPointer<QAbstractItemModel>();
        QVariant variant(model->data(QModelIndex(),
                         (orientation == Qt::Horizontal ? HorizontalHeaderDataRole :
                                                          VerticalHeaderDataRole)));
        if (variant.isValid()) {
            mModel = qobject_cast<QAbstractItemModel*>(variant.value<QObject*>());
        }

        QFontMetrics fm(mHeaderView->font());
        mMinHorizontalCellSize = fm.size(0, mHeaderView->mModelInstance->longestVarText());
        mMinVerticalCellSize = fm.size(0, mHeaderView->mModelInstance->longestEqnText());
        mEmptyTextSize = fm.size(0, "");
    }

    QAbstractItemModel* model() const {
        return mModel;
    }

    bool isValid() const {
        return mModel;
    }

    bool isDimensionFilter(const QPoint &pos) {
        Q_FOREACH(const auto &rect, mDimensionFilterSpots) {
            if (rect.contains(pos))
                return true;
        }
        return false;
    }

    QModelIndex leafIndex(int sectionIndex)
    {
        if (!mModel)
            return QModelIndex();

        int currentLeafIndex = -1;
        for (int i = 0; i < mModel->columnCount(); ++i) {
            QModelIndex index(findLeaf(mModel->index(0, i), sectionIndex,
                                       currentLeafIndex));
            if (index.isValid())
                return index;
        }

        return QModelIndex();
    }

    QModelIndex findLeaf(const QModelIndex &currentIndex, int logiacalLeafIndex,
                         int &currentLeafIndex)
    {
        if (!currentIndex.isValid())
            return QModelIndex();

        int childCount = currentIndex.model()->columnCount(currentIndex);
        if (childCount) {
             for (int i = 0; i < childCount; ++i) {
                 QModelIndex childIndex(findLeaf(mModel->index(0, i, currentIndex),
                                        logiacalLeafIndex,
                                        currentLeafIndex));
                 if (childIndex.isValid())
                     return childIndex;
             }
        } else {
            if (++currentLeafIndex == logiacalLeafIndex)
                return currentIndex;
        }

        return QModelIndex();
    }

    QModelIndexList searchLeafs(const QModelIndex& currentIndex) const
    {
        QModelIndexList leafs;
        if(currentIndex.isValid()) {
            int childCount = 0;
            QModelIndexList tmp { currentIndex };
            while (!tmp.isEmpty()) {
                auto leaf = tmp.takeFirst();
                childCount = leaf.model()->columnCount(leaf);
                if (childCount) {
                    for (int i=0; i<childCount; ++i)
                        tmp.push_back(mModel->index(0, i, leaf));
                } else {
                    leafs.push_back(leaf);
                }
            }
        }
        // TODO which one to keep?
        //int childCount = currentIndex.model()->columnCount(currentIndex);
        //if(childCount) {
        //    for(int i = 0; i < childCount; ++i)
        //        leafs += searchLeafs(mModel->index(0, i, currentIndex));
        //} else {
        //    leafs.push_back(currentIndex);
        //}
        return leafs;
    }

    QModelIndexList subLeafs(const QModelIndex &currentIndex) const
    {
        QModelIndexList leafs;
        if (currentIndex.isValid()) {
            int count = currentIndex.model()->columnCount(currentIndex);
            for (int i=0; i<count; ++i) {
                leafs += searchLeafs(mModel->index(0, i, currentIndex));
            }
        }
        return leafs;
    }

    void paintHorizontalSection(QPainter *painter,
                                const QRect &rect,
                                int logicalLeafIndex,
                                const QStyleOptionHeader &option,
                                const QModelIndex &leafIndex)
    {
        int currentTop = rect.y();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QModelIndexList indexes(branch(leafIndex));
        for (int i=0; i<indexes.size(); ++i) {
            QStyleOptionHeader realStyleOption(option);
            if (i < indexes.size() - 1 &&
                    (realStyleOption.state.testFlag(QStyle::State_Sunken) ||
                     realStyleOption.state.testFlag(QStyle::State_On))) {
                QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
                realStyleOption.state &= (~state);
            }
            currentTop = paintHorizontalCell(painter, rect, logicalLeafIndex,
                                             indexes[i], leafIndex, currentTop,
                                             realStyleOption);
        }

        painter->setBrushOrigin(oldBrushOrigin);
    }

    void paintVerticalSection(QPainter *painter,
                              const QRect &rect,
                              int logicalLeafIndex,
                              QStyleOptionHeader option,
                              const QModelIndex &leafIndex)
    {
        int currentLeft = rect.x();
        QPointF oldBrushOrigin(painter->brushOrigin());

        QModelIndexList indexes(branch(leafIndex));
        for (int i = 0; i < indexes.size(); ++i) {
            QStyleOptionHeader realStyleOption(option);
            if (i < indexes.size() -1 &&
                    (realStyleOption.state.testFlag(QStyle::State_Sunken) ||
                     realStyleOption.state.testFlag(QStyle::State_On))) {
                QStyle::State state(QStyle::State_Sunken | QStyle::State_On);
                realStyleOption.state &= (~state);
            }
            currentLeft = paintVerticalCell(painter, rect, logicalLeafIndex,
                                            indexes[i], leafIndex, currentLeft,
                                            realStyleOption);
        }
        painter->setBrushOrigin(oldBrushOrigin);

    }

    int paintHorizontalCell(QPainter *painter,
                            const QRect &rect,
                            int logicalLeafIndex,
                            const QModelIndex& cellIndex,
                            const QModelIndex &leafIndex,
                            int currentTop,
                            const QStyleOptionHeader &styleOption)
    {
        QStyleOptionHeader option(styleOption);

        int height = cellSize(cellIndex, option).height();
        if (cellIndex == leafIndex) {
            height = rect.height() - currentTop;
        }

        int left = cellLeft(cellIndex, leafIndex, logicalLeafIndex, rect.left());
        int width = cellWidth(cellIndex, leafIndex, logicalLeafIndex);

        option.text = cellIndex.data(Qt::DisplayRole).toString();

        painter->save();
        auto iconSize = pixmapSize();
        if (mHeaderView->isVariable(option.text)) {
            option.rect = QRect(left+iconSize.width(), currentTop, width-iconSize.width(), height);
            QIcon icon(FilterIconOn);
            painter->drawPixmap(left, currentTop, icon.pixmap(iconSize));
            mDimensionFilterSpots.push_back(QRect(QPoint(left, currentTop), iconSize));
        } else {
            option.rect = QRect(left, currentTop, width, height);
        }

        mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &option, painter, mHeaderView);

        //QFontMetrics fm(mHeaderView->font());
        //QSize size = fm.size(0, cellIndex.data().toString());

        if (cellIndex.data().toString() == "z") // TODO macOS cell issue
            qDebug() << "Z RECT: " << option.rect << currentTop;

        //QTransform matrix;
        //matrix.rotate(-90); // TODO mind dim
        //painter->setWorldTransform(matrix, true);
        //
        //QRect labelRect(0, 0, size.height(), size.width());
        //labelRect.moveCenter(QPoint(-rect.center().y(), rect.center().x()));
        //option.rect = labelRect;

        mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &option, painter, mHeaderView);

        painter->restore();

        return currentTop + height;
    }

    int paintVerticalCell(QPainter *painter,
                          const QRect &rect,
                          int logicalLeafIndex,
                          const QModelIndex& cellIndex,
                          const QModelIndex &leafIndex,
                          int currentLeft,
                          const QStyleOptionHeader &styleOption)
    {
        QStyleOptionHeader option(styleOption);

        int width = cellSize(cellIndex, option).width();
        if (cellIndex == leafIndex) {
            width = rect.width() - currentLeft;
        }

        int top = cellLeft(cellIndex, leafIndex, logicalLeafIndex, rect.top());
        int height = cellWidth(cellIndex, leafIndex, logicalLeafIndex);

        option.text = cellIndex.data(Qt::DisplayRole).toString();

        painter->save();
        auto iconSize = pixmapSize();
        if (mHeaderView->isEquation(option.text)) {
            QIcon icon(FilterIconOn);
            painter->drawImage(currentLeft, top, icon.pixmap(iconSize).toImage());
            mDimensionFilterSpots.push_back(QRect(QPoint(currentLeft, top), iconSize));
            option.rect = QRect(currentLeft+iconSize.width(), top, width, height);
        } else {
            if (cellIndex.parent().isValid()) {
                option.rect = QRect(currentLeft+iconSize.width(), top, width, height);
            } else {
                option.rect = QRect(currentLeft, top, width+iconSize.width(), height);
            }
        }

        mHeaderView->style()->drawControl(QStyle::CE_HeaderSection, &option, painter, mHeaderView);
        mHeaderView->style()->drawControl(QStyle::CE_HeaderLabel, &option, painter, mHeaderView);

        painter->restore();

        return currentLeft + width;
    }

    QSize pixmapSize()
    {// TODO dyn. pixmap size
        return QSize(22, 22);
    }

    int cellLeft(const QModelIndex &cellIndex, const QModelIndex &leafIndex,
                 int logicalLeafIndex, int sectionLeft) const
    {
        QModelIndexList leafsList(subLeafs(cellIndex));

        if(leafsList.empty())
            return sectionLeft;
        int n = leafsList.indexOf(leafIndex);
        int firstLeafSectionIndex = logicalLeafIndex - n--;
        for(; n >= 0; --n)
            sectionLeft -= mHeaderView->sectionSize(firstLeafSectionIndex + n);

        return sectionLeft;
    }

    int cellWidth(const QModelIndex &cellIndex, const QModelIndex &leafIndex,
                  int logicalLeafIndex) const
    {
        QModelIndexList leafsList(subLeafs(cellIndex));
        if(leafsList.empty())
            return mHeaderView->sectionSize(logicalLeafIndex);

        int width = 0;
        int firstLeafSectionIndex = logicalLeafIndex - leafsList.indexOf(leafIndex);

        for(int i = 0; i < leafsList.size(); ++i)
            width += mHeaderView->sectionSize(firstLeafSectionIndex + i);

        return width;
    }

    QSize cellSize(const QModelIndex &cellIndex,
                   QStyleOptionHeader styleOption) const
    {
        QSize res;
        QVariant sizeHint(cellIndex.data(Qt::SizeHintRole));
        if (sizeHint.isValid()) {
            res = qvariant_cast<QSize>(sizeHint);
        }

        QFont font(mHeaderView->font());
        QVariant fontRole(cellIndex.data(Qt::FontRole));
        if (fontRole.isValid() && fontRole.canConvert<QFont>()) {
            font = qvariant_cast<QFont>(fontRole);
        }

        font.setBold(true);

        QSize size = mHeaderView->orientation() == Qt::Vertical ? mMinVerticalCellSize :
                                                                  mMinHorizontalCellSize;

        QSize decorationsSize(mHeaderView->style()->sizeFromContents(QStyle::CT_HeaderSection,
                                                                     &styleOption,
                                                                     QSize(),
                                                                     mHeaderView));
        return res.expandedTo(size + decorationsSize - mEmptyTextSize);
    }

    QModelIndexList branch(QModelIndex index) const
    {
        QModelIndexList branch;
        while (index.isValid()) {
            branch.push_front(index);
            index = index.parent();
        }
        return branch;
    }

    QModelIndex root(QModelIndex index) const
    {
        while (index.parent().isValid())
            index = index.parent();
        return index;
    }

private:
    const QString FilterIconOn = ":/img/filter";
    const QString FilterIconOff = ":/img/filter-off";

    const HierarchicalHeaderView *mHeaderView;
    QPointer<QAbstractItemModel> mModel;
    QVector<QRect> mDimensionFilterSpots;

    QSize mMinHorizontalCellSize;
    QSize mMinVerticalCellSize;
    QSize mEmptyTextSize;
};

HierarchicalHeaderView::HierarchicalHeaderView(Qt::Orientation orientation,
                                               QWidget *parent)
    : QHeaderView(orientation, parent)
    , mPrivate(new HierarchicalHeaderView_private(this))
    , mFilterMenu(new QMenu(this))
    , mFilterWidget(new LabelFilterWidget(orientation, this))
{
    setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);

    auto filterAction = new QWidgetAction(mFilterMenu);
    filterAction->setDefaultWidget(mFilterWidget);
    mFilterMenu->addAction(filterAction);

    connect(this, &QHeaderView::sectionResized,
            this, &HierarchicalHeaderView::on_sectionResized);
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
    mPrivate->initFromModel(orientation(), model);
    QHeaderView::setModel(model);
    int count = (orientation() == Qt::Horizontal ? model->columnCount() :
                                                   model->rowCount());
    if(count) {
        initializeSections(0, count-1);
    }
}

void HierarchicalHeaderView::setModelInstance(QSharedPointer<ModelInstance> modelInstance)
{
    mModelInstance = modelInstance;
}

void HierarchicalHeaderView::customMenuRequested(QPoint position)
{
    QStandardItem *item;
    if (orientation() == Qt::Horizontal) {
        item =  mModelInstance->horizontalItem(logicalIndexAt(position));
    } else {
        item = mModelInstance->verticalItem(logicalIndexAt(position));
    }
    if (item) {
        TreeGenerator generator(mModelInstance, orientation());
        auto root = generator.generate(item);
        mFilterWidget->setData(root);
        mFilterMenu->popup(viewport()->mapToGlobal(position));
    }
}

void HierarchicalHeaderView::on_sectionResized(int logicalIndex,
                                               int oldSize,
                                               int newSize)
{
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);

    if (isSectionHidden(logicalIndex))
        return;

    QModelIndex leafIndex(mPrivate->leafIndex(logicalIndex));
    if (!leafIndex.isValid())
        return;

    QModelIndexList subLeafs(mPrivate->subLeafs(mPrivate->root(leafIndex)));
    for (int i=subLeafs.indexOf(leafIndex); i>0; --i) {
        --logicalIndex;

        int width = viewport()->width();
        int height = viewport()->height();
        int pos = sectionViewportPosition(logicalIndex);
        QRect rect(pos, 0, width - pos, height);

        if(orientation() == Qt::Horizontal) {
            if (isRightToLeft())
                rect.setRect(0, 0, pos + sectionSize(logicalIndex), height);
        } else {
            rect.setRect(0, pos, width, height - pos);
        }

        viewport()->update(rect.normalized());
    }
}

void HierarchicalHeaderView::paintSection(QPainter *painter,
                                          const QRect &rect,
                                          int logicalIndex) const
{
    if (rect.isValid()) {
        QModelIndex leafIndex = mPrivate->leafIndex(logicalIndex);
        if (leafIndex.isValid()) {
            auto option = styleOptionForCell(logicalIndex);
            if (orientation() == Qt::Horizontal)
                mPrivate->paintHorizontalSection(painter, rect, logicalIndex,
                                                 option, leafIndex);
            else
                mPrivate->paintVerticalSection(painter, rect, logicalIndex,
                                               option, leafIndex);
            return;
        }
    }

    QHeaderView::paintSection(painter, rect, logicalIndex);
}

QSize HierarchicalHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    if (mPrivate->isValid()) {
        QModelIndex currentIndex(mPrivate->leafIndex(logicalIndex));
        if (currentIndex.isValid()) {
            QStyleOptionHeader styleOption(styleOptionForCell(logicalIndex));
            QSize size(mPrivate->cellSize(currentIndex, styleOption));
            currentIndex = currentIndex.parent();

            while (currentIndex.isValid()) {
                if (orientation() == Qt::Horizontal) {
                    size.rheight() += mPrivate->cellSize(currentIndex,
                                                         styleOption).height();
                } else {
                    size.rwidth() += mPrivate->cellSize(currentIndex, styleOption)
                            .width()+mPrivate->pixmapSize().width();
                }
                currentIndex = currentIndex.parent();
            }
            return size;
        }
    }

    return QHeaderView::sectionSizeFromContents(logicalIndex);
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

bool HierarchicalHeaderView::isEquation(const QString &name) const
{
    return mModelInstance->isEquation(name);
}

bool HierarchicalHeaderView::isVariable(const QString &name) const
{
    return mModelInstance->isVariable(name);
}

}
}
}
