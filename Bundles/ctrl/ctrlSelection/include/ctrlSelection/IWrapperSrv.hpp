/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __CTRLSELECTION_IWRAPPERSRV_HPP__
#define __CTRLSELECTION_IWRAPPERSRV_HPP__

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IWrapperSrv
 * @brief The purpose of this service is to wrap messages incoming to the object to new messages.
 */


class CTRLSELECTION_CLASS_API IWrapperSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IWrapperSrv)(::fwServices::IService) );

    ///@brief IWrapperSrv constructor. Do nothing.
    CTRLSELECTION_API IWrapperSrv();

    ///@brief IWrapperSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IWrapperSrv();

};

}

#endif // __CTRLSELECTION_IWRAPPERSRV_HPP__
