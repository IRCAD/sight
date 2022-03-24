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

#include <service/IXMLParser.hpp>

namespace sight::service
{

namespace parser
{

/**
 * @brief   Specific service that allows to initialize a sight::data::Image.
 * Currently, this only allows to create a default 4x4 2D image with a custom color.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="usExtractedImage" type="sight::data::Image" >
        <color>#00FF00FF</color>
    </object>
   @endcode
 * @see     service::IXMLParser
 */
class SERVICE_CLASS_API Image : public service::IXMLParser
{
public:

    SIGHT_DECLARE_SERVICE(Image, service::IXMLParser);

    SERVICE_API void createConfig(core::tools::Object::sptr _obj) override;
};

} //namespace parser

} //namespace sight::service
