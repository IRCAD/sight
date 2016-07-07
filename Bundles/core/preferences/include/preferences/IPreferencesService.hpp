/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PREFERENCES_IPREFERENCESSERVICE_HPP__
#define __PREFERENCES_IPREFERENCESSERVICE_HPP__

#include "preferences/config.hpp"

#include <fwServices/IService.hpp>

namespace preferences
{

/**
 * @brief Preferences service API.
 * This class represents the base interface for preferences services.
 */
class PREFERENCES_CLASS_API IPreferencesService : public fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (IPreferencesService)(::fwServices::IService) );

    /// Constructor
    IPreferencesService() throw()
    {
    }

    /// Destructor
    virtual ~IPreferencesService() throw()
    {
    }
};

} // namespace preferences

#endif /*__PREFERENCES_IPREFERENCESSERVICE_HPP__*/


