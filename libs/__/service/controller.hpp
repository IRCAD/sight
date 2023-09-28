/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/base.hpp"
#include "service/config.hpp"

namespace sight::service
{

/**
 * @brief   This interface defines control service API.
 * Does nothing particularly, can be considered as a default service type to be implemented by unclassified services.
 */
class SERVICE_CLASS_API controller : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(controller, service::base);

protected:

    ///@brief controller constructor. Do nothing.
    SERVICE_API controller();

    ///@brief controller destructor. Do nothing.
    SERVICE_API ~controller() override;
};

} // namespace sight::service
