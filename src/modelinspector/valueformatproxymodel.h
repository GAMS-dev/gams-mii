#ifndef VALUEFORMATPROXYMODEL_H
#define VALUEFORMATPROXYMODEL_H

#include <QIdentityProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace modelinspector {

class ValueFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    ValueFormatProxyModel(QObject *parent = nullptr);

    ValueFilter valueFilter() const;
    void setValueFilter(const ValueFilter &valueFilter);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QVariant applyFilter(const QVariant &data) const;

private:
    ValueFilter mValueFilter;
    std::function<double(const QVariant &variant, bool *ok)> getValue;
};

}
}
}

#endif // VALUEFORMATPROXYMODEL_H
