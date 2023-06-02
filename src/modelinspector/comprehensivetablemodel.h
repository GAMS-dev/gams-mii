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

class BPOverviewTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BPOverviewTableModel(QObject *parent = nullptr);

    explicit BPOverviewTableModel(int view,
                                        QSharedPointer<AbstractModelInstance> modelInstance,
                                        QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};

class BPCountTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BPCountTableModel(QObject *parent = nullptr);

    explicit BPCountTableModel(int view,
                                     QSharedPointer<AbstractModelInstance> modelInstance,
                                     QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class BPAverageTableModel final : public ComprehensiveTableModel
{
    Q_OBJECT

public:
    explicit BPAverageTableModel(QObject *parent = nullptr);

    explicit BPAverageTableModel(int view,
                                       QSharedPointer<AbstractModelInstance> modelInstance,
                                       QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

}
}
}

#endif // COMPREHENSIVETABLEMODEL_H
