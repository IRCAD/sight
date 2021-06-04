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
#include <service/IAppConfigManager.hpp>
#include <service/IXMLParser.hpp>

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

namespace sight::service
{

namespace parser
{

/**
 * @brief   Specific service for the construction of a Composite and its associated services from an XML-based
 * description.
 * @see     service::IXMLParser
 */
class SERVICE_CLASS_API Composite : public service::IXMLParser
{
public:

    SIGHT_DECLARE_SERVICE(Composite, service::IXMLParser);

    /**
     * @brief   Constructor : does nothing.
     */
    Composite()
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Composite()
    {
    }

    SERVICE_API void createConfig(core::tools::Object::sptr _obj) override;

    SERVICE_API void startConfig() override;

    SERVICE_API void updateConfig() override;

    SERVICE_API void stopConfig() override;

    SERVICE_API void destroyConfig() override;

protected:

    /**
     * @brief   Updating method : create composite object.
     *
     * Parse the composite configuration element to configure and add its objects.
     */
    SERVICE_API void updating() override;

private:

    /// To verify some conditions in xml file
    bool refObjectValidator(core::runtime::ConfigurationElement::sptr _cfgElement);

    std::vector<service::IAppConfigManager::sptr> m_ctmContainer;
};

} //namespace parser

} //namespace sight::service
