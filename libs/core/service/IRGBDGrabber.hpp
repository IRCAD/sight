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

#include "service/config.hpp"
#include <service/IGrabber.hpp>

namespace sight::service
{

/**
 * @brief   This interface defines the RGBD grabber service API.
 *          Must be implemented for services that grabs RGBD camera frames.
 */
class SERVICE_CLASS_API IRGBDGrabber : public service::IGrabber
{
public:

    SIGHT_DECLARE_SERVICE(IRGBDGrabber, service::IGrabber);

    /**
     * @name Data API
     * @{
     */
    SERVICE_API static const service::IService::KeyType s_DEPTHTL_INOUT;
    /** @} */

    /// Constructor.
    SERVICE_API IRGBDGrabber() noexcept;

    /// Destructor.
    SERVICE_API virtual ~IRGBDGrabber() noexcept;

protected:

    data::ptr<data::FrameTL, data::Access::inout> m_depth {this, s_DEPTHTL_INOUT, false, true};
};

} //namespace sight::service
