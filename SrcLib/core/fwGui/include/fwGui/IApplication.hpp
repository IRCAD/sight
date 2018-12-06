/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWGUI_IAPPLICATION_HPP__
#define __FWGUI_IAPPLICATION_HPP__

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the application manipulation API.
 * @class   IApplication
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IApplication : public ::fwGui::GuiBaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IApplication)(::fwGui::GuiBaseObject) )

    /// Constructor. Do nothing.
    FWGUI_API IApplication();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~IApplication();

    /// Tells the application to exit with a returncode
    FWGUI_API virtual void exit( int returncode ) = 0;

    /// Sets if confirmation is required when exiting application
    void setConfirm(bool confirm)
    {
        m_confirm = confirm;
    }

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

protected:
    /// Asks confirmation before exiting application.
    bool m_confirm;
};

} // namespace fwGui

#endif /*__FWGUI_IAPPLICATION_HPP__*/

