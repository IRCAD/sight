/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_SERVICEFACTORYREGISTRY_HPP_
#define _FWSERVICES_SERVICEFACTORYREGISTRY_HPP_

#include <map>

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @class ServiceFactoryInfo
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceFactoryInfo : public ::fwTools::Object
{
    public :

        fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactoryInfo)(::fwTools::Object), (()), ::fwTools::Factory::New< ServiceFactoryInfo > );

        /// Constructor, do nothing.
        FWSERVICES_API ServiceFactoryInfo(){}

        /// Destructor, do nothing.
        FWSERVICES_API virtual ~ServiceFactoryInfo(){}

        std::string serviceType;
        std::string objectImpl;

        ::boost::shared_ptr< ::fwRuntime::Bundle> bundle;
        ::boost::shared_ptr< ::fwTools::TBKClassFactory< ::fwServices::IService, std::pair< std::string, std::string > > > factory;
};


/**
 * @class ServiceFactoryRegistry
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceFactoryRegistry : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactoryRegistry)(::fwCore::BaseObject), (()), new ServiceFactoryRegistry) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ServiceFactoryRegistry::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~ServiceFactoryRegistry();

    /// Parse bundle information to retreive service declaration
    FWSERVICES_API void parseBundleInformation( );

    FWSERVICES_API void addFactory
    (   ::boost::shared_ptr< ::fwTools::IClassFactory > _factory,
        const std::string & simpl,
        const std::string & stype,
        const std::string & oimpl);

    FWSERVICES_API IService::sptr create( const std::string & _srvType, const std::string & _srvImpl );

protected :

    typedef std::map< std::string, ServiceFactoryInfo::sptr > SrvRegContainer;


    /// Container of service information
    SrvRegContainer m_srvImplTosrvInfo;

    /// Constructor, protected to ensure unique instance (singleton pattern)
    FWSERVICES_API ServiceFactoryRegistry();

private :

    /// Helper to print some informations
    void printInfoMap( const SrvRegContainer & src );

    /// Helper to parse bundle information to retreive service declaration
    void parseBundleInformationForObject( SrvRegContainer & _bundleInfoMap );

    /// Helper to trace services not declared in plugin.xml
    void checkServicesNotDeclaredInPluginXml();
};


} // namespace fwServices

#endif // _FWSERVICES_SERVICEFACTORYREGISTRY_HPP_


