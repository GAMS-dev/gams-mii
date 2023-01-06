#ifndef SYMBOLMODELINSTANCETABLEMODEL_H
#define SYMBOLMODELINSTANCETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class SymbolModelInstanceTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Roles {
        RowEntryRole = Qt::UserRole,
        ColumnEntryRole
    };

    SymbolModelInstanceTableModel(QSharedPointer<AbstractModelInstance> modelInstance,
                                   QObject *parent = nullptr);

    void setModelInstance(const QSharedPointer<AbstractModelInstance> &modelInstance);

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    int view() const;

    void setView(int view);

protected:
    QSharedPointer<AbstractModelInstance> mModelInstance;
    int mView;
};

}
}
}

#endif // SYMBOLMODELINSTANCETABLEMODEL_H
