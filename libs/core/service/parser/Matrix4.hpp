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

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

#include <service/IXMLParser.hpp>

namespace sight::service
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a Matrix4 and its associated services from an
 * XML-based description.
 * @see     service::IXMLParser
 */
class SERVICE_CLASS_API Matrix4 : public service::IXMLParser
{

public:

    SIGHT_DECLARE_SERVICE(Matrix4, service::IXMLParser)

    /// Constructor : does nothing.
    Matrix4()
    {
    }

    /// Destructor : does nothing.
    virtual ~Matrix4()
    {
    }

    SERVICE_API void createConfig( core::tools::Object::sptr _obj ) override;
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    SERVICE_API void updating( ) override;

};

} //namespace parser
} //namespace sight::service
