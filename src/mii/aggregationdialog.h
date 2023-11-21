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
#ifndef AGGREGATIONDIALOG_H
#define AGGREGATIONDIALOG_H

#include <QDialog>

#include "aggregation.h"

class QSortFilterProxyModel;

namespace gams {
namespace studio {
namespace mii {

namespace Ui {
class AggregationDialog;
}

class AbstractViewConfiguration;
class FilterTreeItem;

class AggregationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AggregationDialog(QWidget *parent = nullptr);
    ~AggregationDialog();

    QSharedPointer<AbstractViewConfiguration> viewConfig() const;

    void setViewConfig(QSharedPointer<AbstractViewConfiguration> config);

signals:
    void aggregationUpdated();

public slots:
    void on_selectButton_clicked();

    void on_deselectButton_clicked();

    void on_cancelButton_clicked();

    void on_resetButton_clicked();

    void on_applyButton_clicked();

private slots:
    void filterUpdate(const QString &text);

private:
    void setupAggregationView();
    void setupTreeItems(Qt::Orientation orientation, FilterTreeItem *root);

    void applyAggregation();
    AggregationSymbols checkStates(FilterTreeItem *item);
    void applyCheckState(bool state);

private:
    Ui::AggregationDialog *ui;
    int mAggregationMethod = 0;
    bool mAbsValuesGlobal = false;

    QSharedPointer<AbstractViewConfiguration> mViewConfig;

    QSortFilterProxyModel *mAggregationModel = nullptr;

    static QRegularExpression RegExp;
};

}
}
}

#endif // AGGREGATIONDIALOG_H
