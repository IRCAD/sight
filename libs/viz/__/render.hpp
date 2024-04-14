/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/viz/__/config.hpp>

#include <core/tools/failed.hpp>

#include <service/base.hpp>

#include <ui/__/service.hpp>

namespace sight::viz
{

/**
 * @brief   Defines the service interface managing the rendering service for object.
 *
 * Use the base service methods :
 * @li The service is configured with methods set_config(cfg) and configure()
 * @li The method start() initialize the service
 * @li Use update() method to update data
 * @li To react on modifications use update(msg)
 * @li Finally we must call stop() before deleting the service
 *
 */

class SIGHT_VIZ_CLASS_API render : public ui::service
{
public:

    SIGHT_DECLARE_SERVICE(render, ui::service);

protected:

    /**
     * @brief   Constructor. Initialize default values.
     */
    SIGHT_VIZ_API render() noexcept;

    /**
     * @brief   Destructor. Do nothing.
     */
    SIGHT_VIZ_API ~render() noexcept override;
};

} // namespace sight::viz
