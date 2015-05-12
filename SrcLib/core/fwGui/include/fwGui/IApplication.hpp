/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

