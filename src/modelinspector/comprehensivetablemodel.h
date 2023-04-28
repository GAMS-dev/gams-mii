#ifndef COMPREHENSIVETABLEMODEL_H
#define COMPREHENSIVETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

namespace gams {
namespace studio{
namespace modelinspector {

class AbstractModelInstance;

class ComprehensiveTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ComprehensiveTableModel(QObject *parent = nullptr);

    explicit ComprehensiveTableModel(int view,
                                     QSharedPointer<AbstractModelInstance> modelInstance,
                                     QObject *parent = nullptr);

    virtual ~ComprehensiveTableModel();

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

class BlockpicOverviewTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BlockpicOverviewTableModel(QObject *parent = nullptr);

    explicit BlockpicOverviewTableModel(int view,
                                        QSharedPointer<AbstractModelInstance> modelInstance,
                                        QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};

class BlockpicCountTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BlockpicCountTableModel(QObject *parent = nullptr);

    explicit BlockpicCountTableModel(int view,
                                     QSharedPointer<AbstractModelInstance> modelInstance,
                                     QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class BlockpicAverageTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BlockpicAverageTableModel(QObject *parent = nullptr);

    explicit BlockpicAverageTableModel(int view,
                                       QSharedPointer<AbstractModelInstance> modelInstance,
                                       QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

}
}
}

#endif // COMPREHENSIVETABLEMODEL_H
