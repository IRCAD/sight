/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
