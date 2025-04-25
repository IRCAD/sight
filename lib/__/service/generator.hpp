/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <sight/service/config.hpp>

#include <service/base.hpp>

namespace sight::service
{

/**
 * @brief This interface defines generator service API.
 * Must be implemented for services that generate data.
 */
class SIGHT_SERVICE_CLASS_API generator : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(generator, service::base);

protected:

    /// Does nothing.
    SIGHT_SERVICE_API generator();

    /// Destroys the interface.
    SIGHT_SERVICE_API ~generator() override;
};

} // namespace sight::service
