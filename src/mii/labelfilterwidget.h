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
#ifndef GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
#define GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H

#include <QWidget>

#include "common.h"

class QSortFilterProxyModel;

namespace gams {
namespace studio {
namespace mii {

namespace Ui {
class LabelFilterWidget;
}

class FilterTreeItem;

class LabelFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LabelFilterWidget(Qt::Orientation orientation,
                               QWidget *parent = nullptr);
    ~LabelFilterWidget();

    void setData(FilterTreeItem *rootItem);

protected:
    void showEvent(QShowEvent *event);

signals:
    void filterChanged(const gams::studio::mii::IdentifierState&, Qt::Orientation);

private slots:
    void on_applyButton_clicked();

    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void applyFilter(const QString &text);

private:
    void applyCheckState(bool state);
    IdentifierState identifierState();

private:
    Ui::LabelFilterWidget *ui;
    Qt::Orientation mOrientation;
    QSortFilterProxyModel *mFilterModel;
};


} // namespace mii
} // namespace studio
} // namespace gams
#endif // GAMS_STUDIO_MODELINSPECTOR_LABELFILTERWIDGET_H
