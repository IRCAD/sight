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

#ifndef __FWSERVICES_ICONTROLLER_HPP__
#define __FWSERVICES_ICONTROLLER_HPP__

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @brief   This interface defines control service API. Does nothing particularly, can be considered as a default service type to be implemented by unclassified services.
 *
 * @class   IController
 *
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API IController : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IController)(::fwServices::IService) );

protected:

    ///@brief IController constructor. Do nothing.
    FWSERVICES_API IController();

    ///@brief IController destructor. Do nothing.
    FWSERVICES_API virtual ~IController();

};

}

#endif // __FWSERVICES_ICONTROLLER_HPP__
