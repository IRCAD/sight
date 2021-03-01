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

#include <service/IController.hpp>

namespace Tuto05MultithreadConsoleCpp
{

/**
 * @brief This service displays the array values in the log.
 * @note To see the information, you need to activate log level "info".
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::Tuto05MultithreadConsoleCpp::SShowArray" >
            <input key="array" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b array [sight::data::Array]: array which values should be incremented.
 */
class TUTO05MULTITHREADCONSOLECPP_CLASS_API SShowArray final : public sight::service::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SShowArray, sight::service::IController)

    /// Creates the service.
    TUTO05MULTITHREADCONSOLECPP_API SShowArray() noexcept;

    /// Destroys the service.
    TUTO05MULTITHREADCONSOLECPP_API ~SShowArray() noexcept override;

protected:

    /// Does nothing.
    TUTO05MULTITHREADCONSOLECPP_API void configuring() override;

    /// Does nothing.
    TUTO05MULTITHREADCONSOLECPP_API void starting() override;

    /// Displays array values.
    TUTO05MULTITHREADCONSOLECPP_API void updating() override;

    /// Does nothing.
    TUTO05MULTITHREADCONSOLECPP_API void stopping() override;

};

} // namespace Tuto05MultithreadConsoleCpp.
