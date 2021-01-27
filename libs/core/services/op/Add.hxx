/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "services/IService.hpp"
#include "services/registry/ObjectService.hpp"
#include "services/registry/ServiceFactory.hpp"

namespace sight::services
{

//------------------------------------------------------------------------------

template<class SERVICE>
SPTR(SERVICE) add(const std::string& _implType, const std::string& _id)
{
    services::IService::sptr genericSrv = services::add( _implType, _id );
    auto srv                            = std::dynamic_pointer_cast< SERVICE >(genericSrv);
    FW_RAISE_IF("Failed to cast service from factory type '" + _implType + "' into '" +
                core::TypeDemangler<SERVICE>().getClassname() + "'", !srv );

    return srv;
}

//------------------------------------------------------------------------------

}
