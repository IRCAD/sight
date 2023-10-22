/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/base.hpp"

#include <core/tools/object.hpp>

namespace sight::service
{

/**
 * @brief Service type for the construction of an object and associated services from an XML-based description
 *
 * This includes (data) object attributes initialization as well as XML declared service creation, attachment and
 * configuration.
 * As each specific (data) object has a specific structure (attributes), it must be specialized for each one.
 *
 * The updating() method of this base class parses the XML description: each object named XML children corresponds to a
 * field
 * added to the object (see core::tools::object). The New() method on the related child is invoked, therefore
 * allowing to build tree like composite object which services.
 */
class SERVICE_CLASS_API object_parser : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(object_parser, service::base);

    SERVICE_API void setObjectConfig(const service::config_t& _cfg_elem);

    SERVICE_API virtual void createConfig(core::tools::object::sptr _obj);

    SERVICE_API virtual void startConfig();

    SERVICE_API virtual void updateConfig();

    SERVICE_API virtual void stopConfig();

    SERVICE_API virtual void destroyConfig();

    service::config_t m_cfg;

protected:

    /**
     * @brief Constructor. Does nothing.
     */
    SERVICE_API object_parser() = default;

    /**
     * @brief Destructor. Does nothing.
     */
    SERVICE_API ~object_parser() override = default;

    /**
     * @brief Does nothing
     */
    SERVICE_API void starting() override;

    /**
     * @brief Does nothing
     */
    SERVICE_API void stopping() override;

    /**
     * @brief Does nothing
     */
    SERVICE_API void configuring() override;

    /**
     * @brief Parse the XML configuration
     * @note Should invoked ( this->::object_parser::updating() ) from specific XMLParsers updating method to support
     * both
     *       specific compositions and this generic one
     */
    SERVICE_API void updating() override;
};

} // namespace sight::service
