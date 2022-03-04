/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "modules/data/config.hpp"

#include <data/Vector.hpp>

#include <service/IController.hpp>

namespace sight::module::data
{

/**
 * @brief This service is used to extract objects from a vector.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::SGetVector" >
           <in key="vector" uid="..."/>
           <out group="objects">
               <key index="0" uid="..."/>
               <key index="3" uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection Input Input
 * - \b seriesDB [sight::data::Vector]: vector from which the series extracted.
 *
 * @subsection Output Output
 * - \b series [sight::data::Object]: output object extracted from the vector.
 */
class MODULE_DATA_CLASS_API SGetVector : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SGetVector, service::IController);

    /// Constructor
    MODULE_DATA_API SGetVector() = default;

    /// Destructor
    MODULE_DATA_API ~SGetVector() override = default;

protected:

    /// Does nothing
    MODULE_DATA_API void starting() override;

    /// Configure the service
    MODULE_DATA_API void configuring() override;

    /// Does nothing
    MODULE_DATA_API void stopping() override;

    /// Extract the object(s)
    MODULE_DATA_API void updating() override;

private:

    std::vector<size_t> m_indexNumbers;
    sight::data::ptr<sight::data::Vector, sight::data::Access::in> m_vector {this, "vector"};
    sight::data::ptr_vector<sight::data::Object, sight::data::Access::out> m_objects {this, "objects"};
};

} // namespace sight::module::data
