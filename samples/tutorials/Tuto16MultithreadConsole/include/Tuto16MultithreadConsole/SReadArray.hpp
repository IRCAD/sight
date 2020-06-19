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
 * @brief   This service initializes a ::fwData::Array with 10 values (0 to 9).

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SReadArray">
            <inout key="array" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b array [::fwData::Array]: array which values should be initialized.
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SReadArray : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SReadArray, ::fwServices::IController);

    TUTO16MULTITHREADCONSOLE_API SReadArray() noexcept;
    TUTO16MULTITHREADCONSOLE_API virtual ~SReadArray() noexcept;

protected:

    /// Do nothing
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /// Initialize the current ::fwData::Array with 10 values (0 to 9).
    virtual void updating() override;

    /// Do nothing
    virtual void configuring() override;
};

}  // namespace Tuto16MultithreadConsole
