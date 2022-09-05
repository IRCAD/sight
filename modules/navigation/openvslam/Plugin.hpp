/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/navigation/openvslam/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::module::navigation::openvslam
{

/**
 * @brief   This class is started when the modules are loaded.
 */
class MODULE_NAVIGATION_OPENVSLAM_CLASS_API Plugin final : public core::runtime::Plugin
{
public:

    /**
     * @brief destructor
     */
    MODULE_NAVIGATION_OPENVSLAM_API ~Plugin() noexcept final;

    // Overrides
    MODULE_NAVIGATION_OPENVSLAM_API void start() noexcept final;

    // Overrides
    MODULE_NAVIGATION_OPENVSLAM_API void stop() noexcept final;
};

} // namespace sight::module::navigation::openvslam
