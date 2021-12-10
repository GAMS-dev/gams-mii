#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "common.h"

#include <QString>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

class LabelTreeItem;

class SymbolInfo
{
public:
    int offset() const
    {
        return mOffset;
    }

    void setOffset(int offset)
    {
        mOffset = offset;
    }

    int entries() const
    {
        return mEntries;
    }

    void setEntries(int entries)
    {
        mEntries = entries;
    }

    QString name() const
    {
        return mName;
    }

    void setName(const QString &name)
    {
        mName = name;
    }

    int dimension() const
    {
        return mDimension;
    }

    void setDimension(int dimension)
    {
        mDimension = dimension;
    }

    int type() const
    {
        return mType;
    }

    void setType(int type)
    {
        mType = type;
    }

    const SectionLabels& sectionLabels() const
    {
        return mSectionLabels;
    }

    QString label(int sectionIndex, int dimension) const
    {
        auto data = mSectionLabels[sectionIndex];
        if (dimension < data.size())
            return data[dimension];
        return QString();
    }

    void setLabels(int sectionIndex, const QStringList &labels)
    {
        mSectionLabels[sectionIndex] = labels;
    }

    bool contains(int sectionIndex) const
    {
        return sectionIndex >= firstSection() && sectionIndex <= lastSection();
    }

    bool isScalar() const
    {
        return !mDimension;
    }

    int lastOffset() const
    {
        return mOffset+mEntries;
    }

    int firstSection() const {
        return mFirstSection;
    }

    void setFirstSection(int sectionIndex)
    {
        mFirstSection = sectionIndex;
    }

    int lastSection() const
    {
        return mFirstSection+mEntries-1;
    }

    int firstJaccSection() const
    {
        return mFirstSection-PredefinedHeaderLength;
    }

    int lastJaccSection() const
    {
        return lastSection()-PredefinedHeaderLength;
    }

    QSharedPointer<LabelTreeItem> labelTree() const
    {
        return mLabelTree;
    }

    void setLabelTree(QSharedPointer<LabelTreeItem> tree)
    {
        mLabelTree = tree;
    }

private:
    int mFirstSection = -1;
    int mOffset = -1;
    int mEntries = -1;
    QString mName;
    int mDimension = -1;
    int mType = -1;

    SectionLabels mSectionLabels;

    QSharedPointer<LabelTreeItem> mLabelTree = nullptr;
};

}
}
}

#endif // SYMBOLINFO_H
