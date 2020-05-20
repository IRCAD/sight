/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ctrlTF/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ctrlTF
{

/// This class is started when the module is loaded.
struct CTRLTF_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    public:

        /// Destroys the plugin.
        CTRLTF_API virtual ~Plugin() noexcept;

        /// Starts the plugin, does nothing here.
        CTRLTF_API virtual void start() override;

        /// Stops the plugin, does nothing here.
        CTRLTF_API virtual void stop() noexcept override;

};

} // namespace ctrlTF.
