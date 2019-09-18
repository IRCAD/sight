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

#include "fwQml/config.hpp"

#include <fwServices/AppManager.hpp>
#include <fwServices/IService.hpp>

#include <QObject>
#include <QVariant>

namespace fwQml
{

/**
 * @brief   Base class for AppManager managing Qml services
 */
class FWQML_CLASS_QT_API IQmlAppManager : public QObject,
                                          public ::fwServices::AppManager
{
Q_OBJECT
public:
    /// Constructor.
    FWQML_QT_API IQmlAppManager() noexcept;

    /// Destructor. Do nothing.
    FWQML_QT_API virtual ~IQmlAppManager() noexcept;

public Q_SLOTS:

    /// Initialize the manager
    FWQML_QT_API virtual void initialize();

    /// Uninitialize the manager
    FWQML_QT_API virtual void uninitialize();

    /// Retrieves the services instanciated in Qml
    FWQML_QT_API virtual void onServiceCreated(const QVariant& obj);

    /// set the the identifier of the AppManager inputs given by the map
    FWQML_QT_API virtual void replaceInputs(const QVariant& map) final;
};

} // fwQml
