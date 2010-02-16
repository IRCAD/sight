/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_IGUICONTAINER_HPP_
#define _FWWX_IGUICONTAINER_HPP_

#include <wx/window.h>

#include <fwServices/IService.hpp>


#include "fwWX/config.hpp"

/**
 * @brief   The namespace fwWX contains the base interface for wxWidgets services and some specifics wxWidgets components.
 * @namespace   fwWX
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace fwWX
{

/**
 * @brief   Defines the service interface managing the layout.
 * @class   IGuiContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWWX_CLASS_API IGuiContainer : public fwServices::IService
{

public :

    typedef std::map< std::string , wxWindow* > ContainerMapType;

    fwCoreServiceClassDefinitionsMacro ( (IGuiContainer)(::fwServices::IService::Baseclass) ) ;



    /**
     * @brief   Constructor. Initialize default values.
     */
    FWWX_API IGuiContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWWX_API virtual ~IGuiContainer() throw() ;

    /**
     * @name    Helper service methods
     */
    //@{

    /**
     * @brief Returns parent wxContainer
     */
    FWWX_API virtual wxWindow * getWxContainer() ;

    /**
     * @brief Configure parent container associated wit the service (use it before start).
     * If any container has been reserved for this service, parent container will be locally created in a new wxWindow.
     */
    FWWX_API void initGuiParentContainer();

    /**
     * @brief Reset parent wxContainer and destroy it if parent container has been locally created.
     */
    FWWX_API void resetGuiParentContainer();
    /**
     * @brief Registers associate container with specific service's uid in the global and local UID-Container map.
     */
    FWWX_API void registerWxContainer(std::string uid , wxWindow * container);

    /**
     * @brief Unregisters all container in global and local containers (with service UID associated), and stop associated service.
     */
    FWWX_API void unregisterWxContainer(std::string uid);

    /**
     * @brief Unregisters all container in global and local containers, and stop associated services.
     */
    FWWX_API void unregisterAllWxContainer();

    /**
     * @brief Registers container in global container (with service UID associated).
     */
    FWWX_API static void registerGlobalWxContainer(std::string uid, wxWindow * container);

    /**
     * @brief Unregisters container in global container (with service UID associated).
     */
    FWWX_API static void unregisterGlobalWxContainer(std::string uid);

    /**
     * @brief Returns wxContainer associate with service uid.
     */
    FWWX_API static wxWindow* getWxContainer(std::string uid);

    //@}

protected :

    static ContainerMapType m_globalUIDToWxContainer;

    /// Parent WxContainer
    wxWindow* m_container;

    /**
     * @brief Internal sub wxContainer created for specific uid's service
     */
    ContainerMapType m_subUIDToWxContainer;

    /// flag if the container is locally created (ie no parent container associated with this service)
    bool m_isContainerLocallyCreated ;

};

} // namespace fwWX

#endif /*_FWWX_IGUICONTAINER_HPP_*/


