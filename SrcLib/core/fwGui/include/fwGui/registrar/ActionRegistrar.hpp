/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRAR_ACTIONREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRAR_ACTIONREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwMenu.hpp"
#include "fwGui/fwMenuItem.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

namespace registrar
{

/**
 * @brief   Defines the action registrar for IHM.
 * @class   ActionRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ActionRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ActionRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new ActionRegistrar );

    /// Constructor.
    FWGUI_API ActionRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ActionRegistrar();

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    FWGUI_API void actionServiceStarting();

    /// Method called when the action service is activated or not
    FWGUI_API void actionServiceSetActive(bool isActive);

    /// Method called when the action service is executable or not
    FWGUI_API void actionServiceSetExecutable(bool isExecutable);

protected:

    /// Main service ID associate with this ActionRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRAR_ACTIONREGISTRYMANAGER_HPP_*/


