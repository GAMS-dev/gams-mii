#include "hierarchicalheaderview.h"

#include <QPainter>
#include <QPointer>

#include <QDebug>

namespace gams {
namespace studio{
namespace modelinspector {

// TODO
//  o Qt::AlignTop | Qt::AlignLeft not effect for header text
//    -> check if this is a macOS issue)
//  o theme stuff (foreground/background color)

class HierarchicalHeaderView::HierarchicalHeaderView_private
{
public:
    HierarchicalHeaderView_private(const QHeaderView *headerView)
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
    }

    bool isValid() const {
        return mModel;
    }

    QModelIndex leafIndex(int sectionIndex)
    {
        if (!mModel)
            return QModelIndex();

        int currentLeafIndex = -1;
        for (int i = 0; i < mModel->columnCount(); ++i) {
            QModelIndex index(findLeaf(mModel->index(0, i), sectionIndex, currentLeafIndex));
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
        if(!currentIndex.isValid())
            return leafs;
        int childCount = currentIndex.model()->columnCount(currentIndex);
        if(childCount) {
            for(int i = 0; i < childCount; ++i)
                leafs += searchLeafs(mModel->index(0, i, currentIndex));
        } else {
            leafs.push_back(currentIndex);
        }
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

    void paintHorizontalSection(QPainter *painter, const QRect &rect, int logicalLeafIndex,
                                const QStyleOptionHeader &option, const QModelIndex &leafIndex)
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

    void paintVerticalSection(QPainter *painter, const QRect &rect, int logicalLeafIndex,
                              QStyleOptionHeader option, const QModelIndex &leafIndex)
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

    int paintHorizontalCell(QPainter *painter, const QRect &rect, int logicalLeafIndex,
                            const QModelIndex& cellIndex, const QModelIndex &leafIndex,
                            int currentTop, const QStyleOptionHeader &styleOption)
    {
        QStyleOptionHeader option(styleOption);

        int height = cellSize(cellIndex, option).height();
        if (cellIndex == leafIndex) {
            height = rect.height() - currentTop;
        }

        int left = cellLeft(cellIndex, leafIndex, logicalLeafIndex, rect.left());
        int width = cellWidth(cellIndex, leafIndex, logicalLeafIndex);

        option.rect = QRect(left, currentTop, width, height);
        option.text = cellIndex.data(Qt::DisplayRole).toString();
        option.textAlignment = Qt::AlignLeft | Qt::AlignTop;

        painter->save();
        mHeaderView->style()->drawControl(QStyle::CE_Header, &option, painter, mHeaderView);
        painter->restore();

        return currentTop + height;
    }

    int paintVerticalCell(QPainter *painter, const QRect &rect, int logicalLeafIndex,
                          const QModelIndex& cellIndex, const QModelIndex &leafIndex,
                          int currentLeft, const QStyleOptionHeader &styleOption)
    {
        QStyleOptionHeader option(styleOption);

        int width = cellSize(cellIndex, option).width();
        if (cellIndex == leafIndex) {
            width = rect.width() - currentLeft;
        }

        int top = cellLeft(cellIndex, leafIndex, logicalLeafIndex, rect.top());
        int height = cellWidth(cellIndex, leafIndex, logicalLeafIndex);

        option.rect = QRect(currentLeft, top, width, height);
        option.text = cellIndex.data(Qt::DisplayRole).toString();
        option.textAlignment = Qt::AlignLeft | Qt::AlignTop;

        painter->save();
        mHeaderView->style()->drawControl(QStyle::CE_Header, &option, painter, mHeaderView);
        painter->restore();

        return currentLeft + width;
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

    QSize cellSize(const QModelIndex &cellIndex, QStyleOptionHeader styleOption) const
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
        QFontMetrics fontMetrics(font);
        QSize size(fontMetrics.size(0, cellIndex.data(Qt::DisplayRole).toString()));
        QSize decorationsSize(mHeaderView->style()->sizeFromContents(QStyle::CT_HeaderSection,
                                                                     &styleOption,
                                                                     QSize(),
                                                                     mHeaderView));
        QSize emptyTextSize(fontMetrics.size(0, ""));

        return res.expandedTo(size + decorationsSize - emptyTextSize);
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
    const QHeaderView *mHeaderView;
    QPointer<QAbstractItemModel> mModel;
};

HierarchicalHeaderView::HierarchicalHeaderView(Qt::Orientation orientation,
                                               QWidget *parent)
    : QHeaderView(orientation, parent)
    , mPrivate(new HierarchicalHeaderView_private(this))
{
    connect(this, &QHeaderView::sectionResized, this, &HierarchicalHeaderView::on_sectionResized);
}

HierarchicalHeaderView::~HierarchicalHeaderView()
{
    delete mPrivate;
}

void HierarchicalHeaderView::setModel(QAbstractItemModel *model)
{
    mPrivate->initFromModel(orientation(), model);
    QHeaderView::setModel(model);

    int count = (orientation() == Qt::Horizontal ? model->columnCount() : model->rowCount());
    if(count) {
        initializeSections(0, count-1);
    }
}

void HierarchicalHeaderView::on_sectionResized(int logicalIndex, int oldSize, int newSize)
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

void HierarchicalHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (rect.isValid()) {
        QModelIndex leafIndex = mPrivate->leafIndex(logicalIndex);
        if (leafIndex.isValid()) {
            auto option = styleOptionForCell(logicalIndex);
            if (orientation() == Qt::Horizontal)
                mPrivate->paintHorizontalSection(painter, rect, logicalIndex, option, leafIndex);
            else
                mPrivate->paintVerticalSection(painter, rect, logicalIndex, option, leafIndex);
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
                    size.rheight() += mPrivate->cellSize(currentIndex, styleOption).height();
                } else {
                    size.rwidth() += mPrivate->cellSize(currentIndex, styleOption).width();
                }
                currentIndex = currentIndex.parent();
            }
            return size;
        }
    }

    return QHeaderView::sectionSizeFromContents(logicalIndex);
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
            previousSelected = selectionModel()->isColumnSelected(this->logicalIndex(visual - 1), rootIndex());
        else
            previousSelected = selectionModel()->isRowSelected(this->logicalIndex(visual - 1), rootIndex());

        bool nextSelected = false;

        if(orientation() == Qt::Horizontal)
            nextSelected = selectionModel()->isColumnSelected(this->logicalIndex(visual + 1), rootIndex());
        else
            nextSelected = selectionModel()->isRowSelected(this->logicalIndex(visual + 1), rootIndex());

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
