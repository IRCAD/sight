/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_GUIREGISTRY_HPP_
#define _FWGUI_GUIREGISTRY_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/fwContainer.hpp"

/**
 * @brief   The namespace fwGui contains the base interface for IHM services.
 * @namespace   fwGui
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace fwGui
{

/**
 * @brief   This class contains helpers to manage all fwContainer in the application.
 * @class   GuiRegistry.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API GuiRegistry : public ::fwCore::BaseObject
{

public :

    typedef std::map< std::string , ::fwGui::fwContainer::sptr > ContainerMapType;

    fwCoreNonInstanciableClassDefinitionsMacro( (GuiRegistry)(::fwCore::BaseObject) )

    /**
     * @brief   Constructor.
     */
    FWGUI_API GuiRegistry() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~GuiRegistry() ;

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper SID-container methods
     */
    //@{

    /**
     * @brief Registers container associate with service ID.
     */
    FWGUI_API static void registerSIDContainer(std::string sid, ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with service ID.
     */
    FWGUI_API static void unregisterSIDContainer(std::string sid);

    /**
     * @brief Returns fwContainer associate with service ID.
     */
    FWGUI_API static ::fwGui::fwContainer::sptr getSIDContainer(std::string sid);

    //@}

    //-----------------------------------------------------------------------------

    /**
     * @name    Helper WID-container methods
     */
    //@{

    /**
     * @brief Registers container associate with window ID.
     */
    FWGUI_API static void registerWIDContainer(std::string wid, ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters container associate with window ID.
     */
    FWGUI_API static void unregisterWIDContainer(std::string wid);

    /**
     * @brief Returns fwContainer associate with window ID.
     */
    FWGUI_API static ::fwGui::fwContainer::sptr getWIDContainer(std::string wid);

    //@}

    //-----------------------------------------------------------------------------

protected :

    static ContainerMapType m_globalSIDToFwContainer;
    static ContainerMapType m_globalWIDToFwContainer;

};

} // namespace fwGui

#endif /*_FWGUI_GUIREGISTRY_HPP_*/


