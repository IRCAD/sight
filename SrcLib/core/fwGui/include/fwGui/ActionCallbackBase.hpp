/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_ACTIONCALLBACKBASE_HPP__
#define __FWGUI_ACTIONCALLBACKBASE_HPP__

#include "fwGui/config.hpp"
#include "fwGui/IMenuItemCallback.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{

/**
 * @brief   Defines the menu item callback.
 */
class FWGUI_CLASS_API ActionCallbackBase : public ::fwGui::IMenuItemCallback
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ActionCallbackBase)(::fwGui::IMenuItemCallback) )

    typedef std::string RegistryKeyType;

    /**
     * @brief  Sets service ID updated in executed method.
     */
    FWGUI_API virtual void setSID(std::string sid);

    /**
     * @brief  Called method when callback is executed.
     */
    FWGUI_API virtual void execute() override;

    /**
     * @brief  Called method when callback is checked.
     */
    FWGUI_API virtual void check(bool checked);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

protected:

    /**
     * @brief   Constructor.
     */
    FWGUI_API ActionCallbackBase();

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~ActionCallbackBase();

private:
    std::string m_sid;

};

} // namespace fwGui

#endif /*__FWGUI_ACTIONCALLBACKBASE_HPP__*/

