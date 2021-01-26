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

#include "services/config.hpp"
#include <services/IAppConfigManager.hpp>
#include <services/IXMLParser.hpp>

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

namespace sight::services
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a List and its associated services from an XML-based description.
 * @see     services::IXMLParser
 */
class SERVICES_CLASS_API List : public services::IXMLParser
{
public:

    fwCoreServiceMacro(List, services::IXMLParser)

    /**
     * @brief   Constructor : does nothing.
     */
    List( )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~List()
    {
    }

    SERVICES_API void createConfig( core::tools::Object::sptr _obj ) override;

    SERVICES_API void startConfig() override;

    SERVICES_API void updateConfig() override;

    SERVICES_API void stopConfig() override;

    SERVICES_API void destroyConfig() override;

protected:

    /**
     * @brief   Updating method : create List object.
     *
     * Parse the List configuration element to configure and add its objects.
     */
    SERVICES_API virtual void updating( ) override;

private:

    /// To verify some conditions in xml file
    bool refObjectValidator( core::runtime::ConfigurationElement::sptr _cfgElement );

    std::vector< services::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace sight::services
