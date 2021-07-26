#ifndef VALUEFORMATPROXYMODEL_H
#define VALUEFORMATPROXYMODEL_H

#include <QIdentityProxyModel>

#include "valuefiltersettings.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ValueFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    ValueFormatProxyModel(QObject *parent = nullptr);

    void setSettings(const ValueFilterSettings &settings);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QVariant applyFilter(const QVariant &data) const;

private:
    ValueFilterSettings mSettings;
};

}
}
}

#endif // VALUEFORMATPROXYMODEL_H
