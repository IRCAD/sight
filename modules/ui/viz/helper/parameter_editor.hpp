/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <service/base.hpp>

#include <viz/scene3d/parameter_adaptor.hpp>

namespace sight::module::ui::viz::helper
{

class parameter_editor
{
public:

    /**
     * @brief Create a config that will be used to expose an shader parameter adaptor in a parameters.
     *
     * @param[in] _adaptor adaptor to bind.
     * @param[in] _paramSrv editor service.
     * @param[inout] _connections helper that stores connections between the editor and the adaptors.
     */
    MODULE_UI_VIZ_API static service::config_t createConfig(
        const sight::viz::scene3d::parameter_adaptor::csptr& _adaptor,
        const service::base::csptr& _param_srv,
        core::com::helper::sig_slot_connection& _connections
    );
};

} // namespace sight::module::ui::viz::helper
