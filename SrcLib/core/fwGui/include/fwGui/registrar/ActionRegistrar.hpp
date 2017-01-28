/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRAR_ACTIONREGISTRAR_HPP__
#define __FWGUI_REGISTRAR_ACTIONREGISTRAR_HPP__

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace registrar
{

/**
 * @brief   Defines the action registrar for IHM.
 *
 */
class FWGUI_CLASS_API ActionRegistrar : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ActionRegistrar)(::fwGui::GuiBaseObject), (( (const std::string&) )),
                                            new ActionRegistrar );

    /// Constructor.
    FWGUI_API ActionRegistrar( const std::string& sid);

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

    /// Method called when the action service is visible or not
    FWGUI_API void actionServiceSetVisible(bool isVisible);

protected:

    /// Main service ID associate with this ActionRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*__FWGUI_REGISTRAR_ACTIONREGISTRAR_HPP__*/

