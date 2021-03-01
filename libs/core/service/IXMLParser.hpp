/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "service/IService.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Object.hpp>

namespace sight::service
{

/**
 * @brief Service type for the construction of an object and associated services from an XML-based description
 *
 * This includes (data) object attributes initialization as well as XML declared service creation, attachment and
 * configuration.
 * As each specific (data) object has a specific structure (attributes), it must be specialized for each one. Such a
 * service is used by the factory
 * New(core::runtime::ConfigurationElement::sptr ) method.
 *
 * The updating() method of this base class parses the XML description: each object named XML children corresponds to a
 * field
 * added to the object (see core::tools::Object). The New(core::runtime::ConfigurationElement::sptr ) method on the
 * related
 * child is invoked, therefore allowing to build
 * tree like composite object which services.
 *
 *
 */
class SERVICE_CLASS_API IXMLParser : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IXMLParser, service::IService)

public:

    SERVICE_API void setObjectConfig( core::runtime::ConfigurationElement::csptr _cfgElem );

    SERVICE_API virtual void createConfig( core::tools::Object::sptr _obj );

    SERVICE_API virtual void startConfig();

    SERVICE_API virtual void updateConfig();

    SERVICE_API virtual void stopConfig();

    SERVICE_API virtual void destroyConfig();

    core::runtime::ConfigurationElement::csptr m_cfg;

protected:

    /**
     * @brief Constructor. Does nothing.
     */
    SERVICE_API IXMLParser( );

    /**
     * @brief Destructor. Does nothing.
     */
    SERVICE_API virtual ~IXMLParser();

    /**
     * @brief Does nothing
     */
    SERVICE_API virtual void starting() override;

    /**
     * @brief Does nothing
     */
    SERVICE_API virtual void stopping() override;

    /**
     * @brief Does nothing
     */
    SERVICE_API virtual void configuring() override;

    /**
     * @brief Parse the XML configuration
     * @note Should invoked ( this->::IXMLParser::updating() ) from specific XMLParsers updating method to support both
     *       specific compositions and this generic one
     */
    SERVICE_API virtual void updating() override;
};

}
