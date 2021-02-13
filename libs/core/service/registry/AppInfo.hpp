/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "service/config.hpp"

#include <core/BaseObject.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <map>

namespace sight::service
{

namespace registry
{

/// Associations of <pattern, value>.
typedef std::map< std::string, std::string > FieldAdaptorType;

/**
 */
class SERVICE_CLASS_API AppInfo : public core::BaseObject
{
public:

    fwCoreClassMacro(AppInfo, core::BaseObject, new AppInfo)

    /// Constructor, do nothing.
    AppInfo()
    {
    }

    /// Destructor, do nothing.
    ~AppInfo() override
    {
    }

    std::string group;
    std::string desc;
    typedef std::map< std::string, std::string > ParametersType;
    ParametersType parameters;
    core::runtime::ConfigurationElement::csptr config;
    std::string moduleId; ///< Module identifier (used to start the module when the appConfig is launched)
    std::string moduleVersion; ///< Module version (used to start the module when the appConfig is launched)
};

} // namespace registry

} // namespace sight::service
