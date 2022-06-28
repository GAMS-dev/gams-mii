#ifndef ATTRIBUTETABLEMODEL_H
#define ATTRIBUTETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class AttributeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AttributeTableModel(QObject *parent = nullptr);

    virtual ~AttributeTableModel();

    void setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance);

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int view() const
    {
        return mView;
    }

    void setView(int view)
    {
        mView = view;
    }

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    int mView;
};

class EquationAttributeTableModel : public AttributeTableModel
{
    Q_OBJECT

public:
    explicit EquationAttributeTableModel(QObject *parent = nullptr);

    ~EquationAttributeTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
};

class VariableAttributeTableModel : public AttributeTableModel
{
    Q_OBJECT

public:
    explicit VariableAttributeTableModel(QObject *parent = nullptr);

    ~VariableAttributeTableModel();

    QVariant data(const QModelIndex &index, int role) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
};

}
}
}

#endif // ATTRIBUTETABLEMODEL_H
