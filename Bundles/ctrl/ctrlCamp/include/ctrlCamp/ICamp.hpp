/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __CTRLCAMP_ICAMP_HPP__
#define __CTRLCAMP_ICAMP_HPP__

#include "ctrlCamp/config.hpp"

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>



namespace ctrlCamp
{
/**
 * @brief This interface defines service API. It can be considered as a default type for services using fwCamp.
 */
class CTRLCAMP_CLASS_API ICamp : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (ICamp)(::fwServices::IService) );

    /// Constructor
    CTRLCAMP_API ICamp();

    /// Destructor
    CTRLCAMP_API ~ICamp();

};

}// namespace ctrlCamp


#endif /* __CTRLCAMP_ICAMP_HPP__ */
