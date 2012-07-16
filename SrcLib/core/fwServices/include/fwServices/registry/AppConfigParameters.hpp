/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_
#define _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_

#include <map>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/config.hpp"

#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{

namespace registry
{

/**
 * @class AppConfigParameters
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API AppConfigParameters : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfigParameters)(::fwCore::BaseObject), (()), new AppConfigParameters) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static AppConfigParameters::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~AppConfigParameters();

    /// Parse bundle information to retreive service declaration
    FWSERVICES_API void parseBundleInformation();

    FWSERVICES_API const AppConfig::FieldAdaptorType & getParameters( const std::string & extensionId ) const;

    FWSERVICES_API void clearRegistry();

protected :

    typedef std::map< std::string, AppConfig::FieldAdaptorType > Registry;

    /// Container of service information
    Registry m_reg;

    /// Constructor, protected to ensure unique instance (singleton pattern)
    FWSERVICES_API AppConfigParameters();

};

} // namespace registry

} // namespace fwServices

#endif // _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_


