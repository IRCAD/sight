/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ROOTMANAGER_HPP_
#define _FWSERVICES_ROOTMANAGER_HPP_

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include "fwServices/config.hpp"
#include "fwServices/AppConfigManager.hpp"

namespace fwServices
{

class FWSERVICES_CLASS_API RootManager : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (RootManager)(::fwCore::BaseObject), (()), new RootManager) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static RootManager::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~RootManager();

    /**
     * @brief Define the root object XML like configuration name, to be later retrieved from component graph analysis
     */
    FWSERVICES_API static void setRootObjectConfigurationName(std::string name) ;

    FWSERVICES_API static void setRootObjectConfigurationFile(std::string _rootObjectConfigurationFile) ;

    /**
     * @brief Return true if the root object is initialized (i.e. instanciated with attached services, appropriately configured and started and updated)
     */
    FWSERVICES_API static bool isRootObjectInitialized() ;
    /**
     * @brief Trigger root object instanciation, configuration, service attachment, starting and updating
     */
    FWSERVICES_API static void initializeRootObject() ;

    /**
     * @brief Reset root object
     * This involves the stopping of services reported in the configuration and then the stopping of the other. Finally,
     * all services are eliminated as well as objects composing the root object. The object service registry should become empty.
     */
    FWSERVICES_API static void uninitializeRootObject() ;

    /**
     * @brief Return the root object
     */
    FWSERVICES_API static ::fwTools::Object::sptr getRootObject() ;

protected :

    /**
     * @brief Name of the configuration for the root object
     * @note The boolean is false if name undefined as at m_instance initialization.
     */
    std::pair< bool , std::string > m_rootObjectConfigurationName ;

    std::pair< bool , std::string > m_rootObjectConfigurationFile ;

    /**
     * @brief True if root object initialized
     * @note Should be useless as m_rootObject can provide this information
     */
    bool m_isRootInitialized ;

    /**
     * @brief Strong reference of the main application configuration
     */
    ::fwServices::AppConfigManager::sptr m_ctm;

    /// Constructor, protected to ensure unique instance (singleton pattern)
    FWSERVICES_API RootManager();

};

} // namespace fwServices

#endif // _FWSERVICES_ROOTMANAGER_HPP_


