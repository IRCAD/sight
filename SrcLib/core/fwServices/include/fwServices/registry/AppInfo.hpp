/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
