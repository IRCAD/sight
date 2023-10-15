/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "io/__/config.hpp"

#include <io/__/service/grabber.hpp>

namespace sight::io::service
{

/**
 * @brief   This interface defines the RGBD grabber service API.
 *          Must be implemented for services that grabs RGBD camera frames.
 */
class IO_CLASS_API rgbd_grabber : public io::service::grabber
{
public:

    SIGHT_DECLARE_SERVICE(rgbd_grabber, service::grabber);

    /**
     * @name Data API
     * @{
     */
    static constexpr std::string_view s_DEPTHTL_INOUT = "depthTL";
    /** @} */

    /// Constructor.
    IO_API rgbd_grabber() noexcept = default;

    /// Destructor.
    IO_API ~rgbd_grabber() noexcept override = default;

protected:

    data::ptr<data::frame_tl, data::Access::inout> m_depth {this, s_DEPTHTL_INOUT, false, true};
};

} //namespace sight::io::service
