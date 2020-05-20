/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwServices/IController.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief   This service displays the array values in the log.
 *
 * @note To see the information, you need to activate log level "info".
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SShowArray">
            <input key="array" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b array [::fwData::Array]: array which values should be incremented.
 */
/**
 * @brief This service displays the array values.
 *
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SShowArray : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SShowArray, ::fwServices::IController);

    TUTO16MULTITHREADCONSOLE_API SShowArray() noexcept;
    TUTO16MULTITHREADCONSOLE_API virtual ~SShowArray() noexcept;

protected:

    /// Do nothing
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /// Display the array values
    virtual void updating() override;

    /// Do nothing
    virtual void configuring() override;
};

}  // namespace Tuto16MultithreadConsole
