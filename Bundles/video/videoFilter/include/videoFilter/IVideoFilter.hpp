/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "videoFilter/config.hpp"

#include <fwServices/IService.hpp>

namespace videoFilter
{
/**
 * @brief   This interface defines video filters.
 *          Must be implemented for services that filter videos.
 *
 */
class VIDEOFILTER_CLASS_API IVideoFilter : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro((IVideoFilter)(::fwServices::IService));

    /**
     * @brief Constructor.
     */
    IVideoFilter() noexcept
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~IVideoFilter() noexcept
    {
    }

};

} //namespace videoFilter
