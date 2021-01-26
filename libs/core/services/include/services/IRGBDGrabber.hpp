/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "services/config.hpp"
#include <services/IGrabber.hpp>

namespace sight::services
{
/**
 * @brief   This interface defines the RGBD grabber service API.
 *          Must be implemented for services that grabs RGBD camera frames.
 */
class SERVICES_CLASS_API IRGBDGrabber : public services::IGrabber
{

public:

    fwCoreServiceMacro(IRGBDGrabber, services::IGrabber)

    /**
     * @name Data API
     * @{
     */
    SERVICES_API static const services::IService::KeyType s_DEPTHTL_INOUT;
    /** @} */

    /// Constructor.
    SERVICES_API IRGBDGrabber() noexcept;

    /// Destructor.
    SERVICES_API virtual ~IRGBDGrabber() noexcept;
};

} //namespace sight::services
