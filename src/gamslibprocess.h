/**
 * GAMS Model Instance Inspector (MII)
 *
 * Copyright (c) 2023-2026 GAMS Software GmbH <support@gams.com>
 * Copyright (c) 2023-2026 GAMS Development Corp. <support@gams.com>
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
 */
#ifndef GAMSLIBPROCESS_H
#define GAMSLIBPROCESS_H

#include "abstractprocess.h"

class GAMSLibProcess
        : public AbstractProcess
{
    Q_OBJECT

public:
    GAMSLibProcess(QObject *parent = nullptr);

    int modelNumber() const;
    void setModelNumber(int modelNumber);

    void execute() override;

private:
    int mModelNumber = -1;
};

#endif // GAMSLIBPROCESS_H
