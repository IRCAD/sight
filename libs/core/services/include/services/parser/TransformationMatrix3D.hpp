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
#include <services/IXMLParser.hpp>

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

namespace sight::services
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a TransformationMatrix3D and its associated services from an
 * XML-based description.
 * @see     services::IXMLParser
 */
class SERVICES_CLASS_API TransformationMatrix3D : public services::IXMLParser
{

public:

    fwCoreServiceMacro(TransformationMatrix3D, services::IXMLParser)

    /// Constructor : does nothing.
    TransformationMatrix3D()
    {
    }

    /// Destructor : does nothing.
    virtual ~TransformationMatrix3D()
    {
    }

    SERVICES_API void createConfig( core::tools::Object::sptr _obj ) override;
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    SERVICES_API virtual void updating( ) override;

};

} //namespace parser
} //namespace sight::services
