/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwPreferences/config.hpp"

#include <fwServices/IService.hpp>

namespace fwPreferences
{

/**
 * @brief Preferences service API.
 * This class represents the base interface for preferences services.
 */
class FWPREFERENCES_CLASS_API IPreferences : public fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro( (IPreferences)(::fwServices::IService) );

    /// Constructor
    IPreferences() noexcept
    {
    }

    /// Destructor
    virtual ~IPreferences() noexcept
    {
    }
};

} // namespace fwPreferences
