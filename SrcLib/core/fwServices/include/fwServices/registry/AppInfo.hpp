/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_APPINFO_HPP__
#define __FWSERVICES_REGISTRY_APPINFO_HPP__

#include "fwServices/config.hpp"

#include <fwCore/BaseObject.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <map>

namespace fwServices
{

namespace registry
{

/// Associations of <pattern, value>.
typedef std::map< std::string, std::string > FieldAdaptorType;

/**
 */
class FWSERVICES_CLASS_API AppInfo : public ::fwCore::BaseObject
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppInfo)(::fwCore::BaseObject), (()), new AppInfo );

    /// Constructor, do nothing.
    AppInfo()
    {
    }

    /// Destructor, do nothing.
    virtual ~AppInfo()
    {
    }

    std::string group;
    std::string desc;
    typedef std::map< std::string, std::string > ParametersType;
    ParametersType parameters;
    ::fwRuntime::ConfigurationElement::csptr config;
    std::string bundleId; ///< Bundle identifier (used to start the bundle when the appConfig is launched)
    std::string bundleVersion; ///< Bundle version (used to start the bundle when the appConfig is launched)
};

} // namespace registry

} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_APPINFO_HPP__
