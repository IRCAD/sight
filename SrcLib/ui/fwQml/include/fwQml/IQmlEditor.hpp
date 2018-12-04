/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include <fwServices/IService.hpp>

#include <QObject>

namespace fwQml
{

/**
 * @brief Defines the base class for service editing an object associated to a qml UI.
 *
 * Don't forget to register your editor in you bundle Plugin::start()
 * @code{.cpp}
    qmlRegisterType<MyEditor>("myBundle", versionMajor, versionMinor, "MyEditor");
   @endcode
 */
class FWQML_CLASS_QT_API IQmlEditor : public QObject,
                                      public ::fwServices::IService
{

Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (IQmlEditor)(::fwServices::IService) )

    /// Constructor. Do nothing.
    FWQML_QT_API IQmlEditor() noexcept;

    /// Destructor. Do nothing.
    FWQML_QT_API virtual ~IQmlEditor() noexcept;

Q_SIGNALS:

    /// Signal emitted when the service is started
    void started();

    /// Signal emitted when the service is stopped
    void stopped();

protected:

    /// call updating and emit 'started' qt signal
    FWQML_QT_API virtual void starting() override;

    /// emit 'stopped' qt signal
    FWQML_QT_API virtual void stopping() override;

};

} // namespace fwQml
