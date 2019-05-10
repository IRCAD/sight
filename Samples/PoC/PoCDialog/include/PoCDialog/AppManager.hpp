/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "PoCDialog/config.hpp"

#include <fwServices/AppManager.hpp>

#include <QObject>

namespace PoCDialog
{

/**
 * @brief   This class manages the services used by the Application. It is launched by the qml file 'ui.qml'
 */
class POCDIALOG_CLASS_API AppManager : public QObject,
                                       public ::fwServices::AppManager
{

Q_OBJECT
public:
    /// Constructor.
    POCDIALOG_API AppManager() noexcept;

    /// Destructor. Do nothing.
    POCDIALOG_API ~AppManager() noexcept;

public Q_SLOTS:
    void initialize();
    void uninitialize();

Q_SIGNALS:

private:

};

} // namespace PoCDialog
