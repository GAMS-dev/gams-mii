#ifndef SYMBOL_H
#define SYMBOL_H

#include "common.h"

#include <QString>
#include <QSharedPointer>

namespace gams {
namespace studio {
namespace modelinspector {

class LabelTreeItem;

class Symbol
{
public:
    enum Type
    {
        Equation,
        Variable,
        Unknown
    };

    int offset() const;

    void setOffset(int offset);

    int entries() const;

    void setEntries(int entries);

    QString name() const;

    void setName(const QString &name);

    int dimension() const;

    void setDimension(int dimension);

    Type type() const;

    void setType(Type type);

    const DomainLabels &domainLabels() const;

    void setDomainLabels(const DomainLabels &labels);

    void appendDomainLabel(const QString &label);

    const SectionLabels& sectionLabels() const;

    QString label(int sectionIndex, int dimension) const;

    void setLabels(int sectionIndex, const QStringList &labels);

    bool contains(int sectionIndex) const;

    bool isScalar() const;

    int lastOffset() const;

    int firstSection() const;

    void setFirstSection(int sectionIndex);

    int lastSection() const;

    int logicalIndex() const;

    void setLogicalIndex(int index);

    QSharedPointer<LabelTreeItem> labelTree() const;

    void setLabelTree(QSharedPointer<LabelTreeItem> tree);

    bool isEquation() const;

    bool isVariable() const;

    bool operator==(const Symbol &other) const;

    bool operator!=(const Symbol &other) const;

private:
    int mFirstSection = -1;
    int mOffset = -1;
    int mEntries = -1;
    QString mName;
    int mDimension = -1;
    Type mType = Unknown;
    int mLogicalIndex = -1;

    SectionLabels mSectionLabels;

    DomainLabels mDomainLabels;

    QSharedPointer<LabelTreeItem> mLabelTree = nullptr;
};

}
}
}

#endif // SYMBOL_H
