/************************************************************************
 *
 * Copyright (C) 2014-2016 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#ifndef __ARSERVICES_ISIMULATOR_HPP__
#define __ARSERVICES_ISIMULATOR_HPP__

#include "arServices/config.hpp"

#include <fwServices/IService.hpp>

namespace arServices
{

/**
 * @brief   This interface defines simulator service API.
 *          Must be implemented for services that simulate.
 */
class ARSERVICES_CLASS_API ISimulator : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ISimulator)(::fwServices::IService) );

protected:

    /// ISimulator constructor. Do nothing.
    ARSERVICES_API ISimulator();

    /// ISimulator destructor. Do nothing.
    ARSERVICES_API virtual ~ISimulator();

};

}

#endif // __ARSERVICES_ISIMULATOR_HPP__
