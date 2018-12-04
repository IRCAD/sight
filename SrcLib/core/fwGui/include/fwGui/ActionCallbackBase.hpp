/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

