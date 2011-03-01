/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_REGISTRY_SERVICECONFIG_HPP_
#define _FWSERVICES_REGISTRY_SERVICECONFIG_HPP_

#include <map>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{

namespace registry
{


/**
 * @class ServiceConfigInfo
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceConfigInfo : public ::fwCore::BaseObject
{
public :

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceConfigInfo)(::fwCore::BaseObject), (()), new ServiceConfigInfo );

    /// Constructor, do nothing.
    FWSERVICES_API ServiceConfigInfo(){}

    /// Destructor, do nothing.
    FWSERVICES_API virtual ~ServiceConfigInfo(){}

    std::string service;
    std::string desc;
    ::fwRuntime::ConfigurationElement::csptr config;
};


/**
 * @class ServiceConfig
 * @author  IRCAD (Research and Development Team).
 *
 *
 * Registry config like :
 * @verbatim
<extension implements="::fwServices::registry::ServiceConfig">
    <id>IOSelectorVRMedPatientDBWriter</id>
    <service>::uiIO::editor::IOSelectorService</service>
    <desc>IOSelector config for VRMed patientDB writer</desc>
    <config>
        <type mode="writer" />
        <selection mode="exclude" />
        <addSelection service="::ioXML::FwXMLGenericWriterService" />
        <addSelection service="::uiDicomViewer::DicomPatientDBWriterService" />
    </config>
</extension>
   @endverbatim
 */
class FWSERVICES_CLASS_API ServiceConfig : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceConfig)(::fwCore::BaseObject), (()), new ServiceConfig) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ServiceConfig::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~ServiceConfig();

    /// Parse bundle information to retreive service declaration
    FWSERVICES_API void parseBundleInformation();

    FWSERVICES_API void addServiceConfigInfo( const std::string & configId,
                                              const std::string & service,
                                              const std::string & desc,
                                              ::fwRuntime::ConfigurationElement::csptr config );

    /// Returns the configuration with the given id for the service with the given implementation
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getServiceConfig( const std::string & configId, const std::string &serviceImpl="" ) const;

    /// Returns a vector of config for the service with the given implementation
    FWSERVICES_API std::vector< std::string > getAllConfigForService( std::string serviceImpl ) const;

    FWSERVICES_API void clearRegistry();

protected :

    typedef std::map< std::string, ServiceConfigInfo::sptr > Registry;

    /// Container of service information
    Registry m_reg;

    /// Constructor, protected to ensure unique instance (singleton pattern)
    FWSERVICES_API ServiceConfig();

    const static std::string CONFIG_EXT_POINT;

};

} // namespace registry

} // namespace fwServices

#endif // _FWSERVICES_REGISTRY_SERVICECONFIG_HPP_


