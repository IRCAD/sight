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
#include <service/IService.hpp>

namespace sight::service
{

/**
 * @brief   This interface defines simulator service API.
 *          Must be implemented for services that simulate.
 */
class SERVICE_CLASS_API ISimulator : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(ISimulator, service::IService);

protected:

    /// ISimulator constructor. Do nothing.
    SERVICE_API ISimulator();

    /// ISimulator destructor. Do nothing.
    SERVICE_API virtual ~ISimulator();
};

}
