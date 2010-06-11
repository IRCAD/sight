/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IGUICONTAINER_HPP_
#define _FWGUI_IGUICONTAINER_HPP_

#include <fwServices/IService.hpp>

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
 * @brief   Defines the service interface managing the layout.
 * @class   IGuiContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IGuiContainer : public ::fwServices::IService
{

public :

    typedef std::map< std::string , ::fwGui::fwContainer::sptr > ContainerMapType;

    fwCoreServiceClassDefinitionsMacro ( (IGuiContainer)(::fwServices::IService) ) ;

    /**
     * @name    Helper service methods
     */
    //@{

    /**
     * @brief Returns parent fwContainer
     */
    FWGUI_API virtual ::fwGui::fwContainer::sptr getContainer() ;

    /**
     * @brief Configure parent container associated wit the service (use it before start).
     * If any container has been reserved for this service, parent container will be locally created in a new fwContainer.
     */
    FWGUI_API void initGuiParentContainer();

    /**
     * @brief Reset parent fwContainer and destroy it if parent container has been locally created.
     */
    FWGUI_API void resetGuiParentContainer();
    /**
     * @brief Registers associate container with specific service's uid in the global and local UID-Container map.
     */
    FWGUI_API void registerContainer(std::string uid , ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters all container in global and local containers (with service UID associated), and stop associated service.
     */
    FWGUI_API void unregisterContainer(std::string uid);

    /**
     * @brief Unregisters all container in global and local containers, and stop associated services.
     */
    FWGUI_API void unregisterAllContainer();

    /**
     * @brief Registers container in global container (with service UID associated).
     */
    FWGUI_API static void registerGlobalContainer(std::string uid, ::fwGui::fwContainer::sptr container);

    /**
     * @brief Unregisters container in global container (with service UID associated).
     */
    FWGUI_API static void unregisterGlobalContainer(std::string uid);

    /**
     * @brief Returns fwContainer associate with service uid.
     */
    FWGUI_API static ::fwGui::fwContainer::sptr getContainer(std::string uid);

    //@}

protected :

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IGuiContainer() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IGuiContainer() ;


    static ContainerMapType m_globalUIDToContainer;

    /// Parent Container
    ::fwGui::fwContainer::sptr m_container;

    /**
     * @brief Internal sub fwContainer created for specific uid's service
     */
    ContainerMapType m_subUIDToContainer;
};

} // namespace fwGui

#endif /*_FWGUI_IGUICONTAINER_HPP_*/


