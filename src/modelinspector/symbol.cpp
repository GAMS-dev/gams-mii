#include "symbol.h"
#include "labeltreeitem.h"

namespace gams {
namespace studio {
namespace modelinspector {

int Symbol::offset() const
{
    return mOffset;
}

void Symbol::setOffset(int offset)
{
    mOffset = offset;
}

int Symbol::entries() const
{
    return mEntries;
}

void Symbol::setEntries(int entries)
{
    mEntries = entries;
}

QString Symbol::name() const
{
    return mName;
}

void Symbol::setName(const QString &name)
{
    mName = name;
}

int Symbol::dimension() const
{
    return mDimension;
}

void Symbol::setDimension(int dimension)
{
    mDimension = dimension;
}

Symbol::Type Symbol::type() const
{
    return mType;
}

void Symbol::setType(Type type)
{
    mType = type;
}

const DomainLabels &Symbol::domainLabels() const
{
    return mDomainLabels;
}

void Symbol::setDomainLabels(const DomainLabels &labels)
{
    mDomainLabels = labels;
}

void Symbol::appendDomainLabel(const QString &label)
{
    mDomainLabels.push_back(label);
}

const SectionLabels &Symbol::sectionLabels() const
{
    return mSectionLabels;
}

QString Symbol::label(int sectionIndex, int dimension) const
{
    auto data = mSectionLabels[sectionIndex];
    if (dimension < data.size())
        return data[dimension];
    return QString();
}

void Symbol::setLabels(int sectionIndex, const QStringList &labels)
{
    mSectionLabels[sectionIndex] = labels;
}

bool Symbol::contains(int sectionIndex) const
{
    return sectionIndex >= firstSection() && sectionIndex <= lastSection();
}

bool Symbol::isScalar() const
{
    return !mDimension;
}

int Symbol::lastOffset() const
{
    return mOffset+mEntries;
}

int Symbol::firstSection() const {
    return mFirstSection;
}

void Symbol::setFirstSection(int sectionIndex)
{
    mFirstSection = sectionIndex;
}

int Symbol::lastSection() const
{
    return mFirstSection+mEntries-1;
}

int Symbol::logicalIndex() const
{
    return mLogicalIndex;
}

void Symbol::setLogicalIndex(int index)
{
    mLogicalIndex = index;
}

QSharedPointer<LabelTreeItem> Symbol::labelTree() const
{
    return mLabelTree;
}

void Symbol::setLabelTree(QSharedPointer<LabelTreeItem> tree)
{
    mLabelTree = tree;
}

bool Symbol::isEquation() const
{
    return mType == Equation;
}

bool Symbol::isVariable() const
{
    return mType == Variable;
}

bool Symbol::operator==(const Symbol &other) const
{
    return mFirstSection == other.mFirstSection     &&
           mOffset == other.mOffset                 &&
           mEntries == other.mEntries               &&
           mName == other.mName                     &&
           mDimension == other.mDimension           &&
           mType == other.mType                     &&
           mDomainLabels == other.mDomainLabels     &&
           mSectionLabels == other.mSectionLabels   &&
           mLabelTree.get() == other.mLabelTree.get();
}

bool Symbol::operator!=(const Symbol &other) const
{
    return !(*this==other);
}

}
}
}
