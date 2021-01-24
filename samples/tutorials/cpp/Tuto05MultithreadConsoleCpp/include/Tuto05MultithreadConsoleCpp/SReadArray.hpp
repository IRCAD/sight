/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "Tuto05MultithreadConsoleCpp/config.hpp"

#include <fwServices/IController.hpp>

namespace Tuto05MultithreadConsoleCpp
{

/**
 * @brief This service initializes a data::Array with 10 values (0 to 9).
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::Tuto05MultithreadConsoleCpp::SReadArray" >
            <inout key="array" uid="..." />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b array [data::Array]: array which values should be initialized.
 */
class TUTO05MULTITHREADCONSOLECPP_CLASS_API SReadArray final : public ::fwServices::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SReadArray, ::fwServices::IController)

    /// Creates the service.
    TUTO05MULTITHREADCONSOLECPP_API SReadArray() noexcept;

    /// Destroys the service.
    TUTO05MULTITHREADCONSOLECPP_API ~SReadArray() noexcept override;

protected:

    /// Does nothing.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Initializes the current data::Array with 10 values (0 to 9).
    void updating() override;

    /// Does nothing.
    void stopping() override;

};

} // namespace Tuto05MultithreadConsoleCpp.
