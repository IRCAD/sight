/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPREFERENCES_IPREFERENCES_HPP__
#define __FWPREFERENCES_IPREFERENCES_HPP__

#include "fwPreferences/config.hpp"

#include <fwServices/IService.hpp>

namespace fwPreferences
{

/**
 * @brief Preferences service API.
 * This class represents the base interface for preferences services.
 */
class PREFERENCES_CLASS_API IPreferences : public fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (IPreferences)(::fwServices::IService) );

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

#endif /*__FWPREFERENCES_IPREFERENCES_HPP__*/


