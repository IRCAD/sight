/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/runtime/ConfigurationElement.hpp>

#include <service/AppConfigManager.hpp>
#include <service/IXMLParser.hpp>

namespace sight::service::parser
{

/**
 * @brief   Specific service for the construction of a Vector and its associated services from an XML-based description.
 * @see     service::IXMLParser
 */
class SERVICE_CLASS_API Vector : public service::IXMLParser
{
public:

    SIGHT_DECLARE_SERVICE(Vector, service::IXMLParser);

    /**
     * @brief   Constructor : does nothing.
     */
    Vector()
    = default;

    /**
     * @brief   Destructor : does nothing.
     */
    ~Vector() override
    = default;

    SERVICE_API void createConfig(core::tools::Object::sptr _obj) override;

    SERVICE_API void startConfig() override;

    SERVICE_API void updateConfig() override;

    SERVICE_API void stopConfig() override;

    SERVICE_API void destroyConfig() override;

protected:

    /**
     * @brief   Updating method : create Vector object.
     *
     * Parse the Vector configuration element to configure and add its objects.
     */
    SERVICE_API void updating() override;

private:

    /// To verify some conditions in xml file
    static bool refObjectValidator(core::runtime::ConfigurationElement::sptr _cfgElement);

    std::vector<service::IAppConfigManager::sptr> m_ctmContainer;
};

} // namespace sight::service::parser
