/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <core/tools/Object.hpp>

#include <services/IXMLParser.hpp>

namespace sight::services
{
namespace parser
{

/**
 * @brief This service parse an hexa color value and set it to the color data.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="..." type="data::Color" >
        <value>#FFFFFF</value>
    </object>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b value (optional, hexadecimal, default=#FFFFFF): color sets to the data.
 */
class SERVICES_CLASS_API Color final : public services::IXMLParser
{

public:

    /// Creates default attributes and methods (New(), ::sptr, ...).
    fwCoreServiceMacro(Color, services::IXMLParser)

    /// Does nothing.
    SERVICES_API Color()
    {
    }

    /// Does nothing.
    SERVICES_API virtual ~Color()
    {
    }

private:

    /**
     * @brief Parse a color XML configuration.
     * @param _obj color in which apply the configuration.
     */
    void createConfig(core::tools::Object::sptr _obj) override;

};

} // namespace parser.
} // namespace sight::services.
