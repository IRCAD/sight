/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/app/config.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief   Specific service for the construction of a transfer_function and its associated services from an XML-based
 * description.
 * @see     service::object_parser
 */
class SIGHT_APP_CLASS_API transfer_function : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(transfer_function, service::object_parser);

    /// Constructor : does nothing.
    transfer_function()
    = default;

    /// Destructor : does nothing.
    ~transfer_function() override
    = default;

    /**
     * @code{.xml}
       <object type="data::transfer_function">
           <name>Bones</name>
           <colors isClamped="true">
               <step color="#ffffff" value="0" />
               <step color="#0000ff" value="1000" />
               <step color="#00ff00" value="2000" />
               <step color="#ffff00" value="3000" />
               <step color="#ff0000" value="4000" />
               <step color="#000000" value="5000" />
           </colors>
       </object>
       @endcode
       With:
     * - \b name (optional, default: '') : the name of the TF.
     * - \b step : defines a step in the legend given with the arguments color in hex-code and the max value.
     * - \b isClamped(optional, default: true) : defines interpolation mode on extremities, if yes then after extremity
     *       point, the returned TF color is color_t(0,0,0,0), else it is the color value of the extremity.
     *
     * You can also define a default TF this way:
     *
     * @code{.xml}
       <object type="data::transfer_function">
           <colors default="true" />
       </object>
       @endcode
     *
     */
    SIGHT_APP_API void create_config(core::object::sptr _obj) override;

protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    SIGHT_APP_API void updating() override;
};

} // namespace sight::app::parser
