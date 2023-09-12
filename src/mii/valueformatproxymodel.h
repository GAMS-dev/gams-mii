/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023 GAMS Development Corp. <support@gams.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef VALUEFORMATPROXYMODEL_H
#define VALUEFORMATPROXYMODEL_H

#include <QIdentityProxyModel>

#include "common.h"

namespace gams {
namespace studio {
namespace mii {

class ValueFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    ValueFormatProxyModel(QObject *parent = nullptr);

    virtual void setValueFilter(const ValueFilter &valueFilter);

    virtual QVariant data(const QModelIndex &index, int role) const = 0;

protected:
    ValueFilter mValueFilter;
};

class GeneralValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    GeneralValueFormatProxyModel(QObject *parent = nullptr);

    void setValueFilter(const ValueFilter &valueFilter) override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QVariant applyFilter(const QVariant &data) const;

private:
    std::function<double(const QVariant &variant, bool *ok)> getValue;
};

class JacobianValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    JacobianValueFormatProxyModel(QObject *parent = nullptr);

    void setValueFilter(const ValueFilter &valueFilter) override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    std::function<double(const QVariant &variant, bool *ok)> getValue;
};

class BPValueFormatProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    BPValueFormatProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class BPValueFormatTypeProxyModel final : public ValueFormatProxyModel
{
    Q_OBJECT

public:
    BPValueFormatTypeProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
};

class AbsFormatProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    AbsFormatProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    void setAbsFormat(bool absoluteValues);

private:
    bool mAbsoluteValues = false;
};

}
}
}

#endif // VALUEFORMATPROXYMODEL_H
