/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <data/vector.hpp>

#include <service/controller.hpp>

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
        <service uid="..." type="sight::module::data::get_vector" >
           <in key="vector" uid="..."/>
           <out group="objects">
               <key index="0" uid="..."/>
               <key index="3" uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection Input Input
 * - \b vector [sight::data::vector]: vector from which the series extracted.
 *
 * @subsection Output Output
 * - \b series [sight::data::object]: output object extracted from the vector.
 */
class get_vector : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(get_vector, service::controller);

    /// Constructor
    get_vector() = default;

    /// Destructor
    ~get_vector() override = default;

protected:

    /// Does nothing
    void starting() override;

    /// Configure the service
    void configuring() override;

    /// Does nothing
    void stopping() override;

    /// Extract the object(s)
    void updating() override;

private:

    std::vector<size_t> m_index_numbers;
    sight::data::ptr<sight::data::vector, sight::data::access::in> m_vector {this, "vector"};
    sight::data::ptr_vector<sight::data::object, sight::data::access::out> m_objects {this, "objects"};
};

} // namespace sight::module::data
