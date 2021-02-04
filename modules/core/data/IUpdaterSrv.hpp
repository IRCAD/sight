/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/data/config.hpp"

#include <data/Composite.hpp>

#include <services/IService.hpp>

#include <tuple>

namespace sight::modules::data
{

/**
 * @brief   An updater is use to register/unregister object in the OSR when it receives specific signals (declared in
 * configuration).
 */
class MODULE_DATA_CLASS_API IUpdaterSrv : public services::IService
{

public:

    fwCoreServiceMacro(IUpdaterSrv, services::IService)

    ///@brief IUpdaterSrv constructor. Do nothing.
    MODULE_DATA_API IUpdaterSrv();

    ///@brief IUpdaterSrv destructor. Do nothing.
    MODULE_DATA_API virtual ~IUpdaterSrv();
};

}
