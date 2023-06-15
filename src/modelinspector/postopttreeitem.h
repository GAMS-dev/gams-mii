#ifndef POSTOPTTREEITEM_H
#define POSTOPTTREEITEM_H

#include <QVector>
#include <QVariant>

namespace gams {
namespace studio {
namespace modelinspector {

class PostoptTreeItem
{
public:
    explicit PostoptTreeItem(PostoptTreeItem* parent = nullptr)
        : mParent(parent)
    {

    }

    virtual ~PostoptTreeItem()
    {
        qDeleteAll(mChilds);
    }

    virtual QVariant data(int index) const = 0;

    void append(PostoptTreeItem *child)
    {
        if (child)
            mChilds.append(child);
    }

    PostoptTreeItem* child(int index) const
    {
        if (index < 0 || index >= mChilds.size())
            return nullptr;
        return mChilds.at(index);
    }

    int rowCount() const
    {
        return mChilds.size();
    }

    virtual int columnCount() const = 0;

    int row() const
    {
        if (mParent)
            return mParent->mChilds.indexOf(const_cast<PostoptTreeItem*>(this));
        return 0;
    }

    PostoptTreeItem* parent() const
    {
        return mParent;
    }

    void setParent(PostoptTreeItem* parent)
    {
        mParent = parent;
    }

    static const QString AttributeHeader;
    static const QString EquationHeader;
    static const QString VariableHeader;
    static const QVector<QVariant> AttributeRowHeader;
    static const QVector<QVariant> EquationLineHeader;
    static const QVector<QVariant> VariableLineHeader;

private:
    PostoptTreeItem *mParent;

    QVector<PostoptTreeItem*> mChilds;
};

class GroupPostoptTreeItem : public PostoptTreeItem
{
public:
    explicit GroupPostoptTreeItem(const QString &value = QString(), PostoptTreeItem* parent = nullptr)
        : PostoptTreeItem(parent)
        , mValue(value)
    {

    }

    QVariant data(int index) const override
    {
        return index == 0 ? mValue : QVariant();
    }

    int columnCount() const override
    {
        return 1;
    }

private:
    QString mValue;
};

class LinePostoptTreeItem : public PostoptTreeItem
{
public:
    explicit LinePostoptTreeItem(const QVector<QVariant>& data = QVector<QVariant>(),
                                 PostoptTreeItem* parent = nullptr)
        : PostoptTreeItem(parent)
        , mData(data)
    {

    }

    QVariant data(int index) const override
    {
        if (index < 0 || index >= mData.size())
            return QVariant();
        return mData[index];
    }

    int columnCount() const override
    {
        return mData.size();
    }

private:
    QVector<QVariant> mData;
};

}
}
}

#endif // POSTOPTTREEITEM_H
