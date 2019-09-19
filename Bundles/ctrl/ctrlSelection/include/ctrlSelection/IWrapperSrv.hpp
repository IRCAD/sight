/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "ctrlSelection/config.hpp"

#include <fwServices/IService.hpp>

namespace ctrlSelection
{

/**
 * @brief The purpose of this service is to wrap messages incoming to the object to new messages.
 */

class CTRLSELECTION_CLASS_API IWrapperSrv : public ::fwServices::IService
{

public:

    fwCoreServiceMacro(IWrapperSrv, ::fwServices::IService);

    ///@brief IWrapperSrv constructor. Do nothing.
    CTRLSELECTION_API IWrapperSrv();

    ///@brief IWrapperSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IWrapperSrv();

};

}
