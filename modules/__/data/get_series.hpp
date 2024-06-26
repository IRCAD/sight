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

#include <core/base.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/controller.hpp>
#include <service/macros.hpp>

namespace sight::module::data
{

/**
 * @brief This service is used to  extracts a series in a series database
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::get_series" >
           <in key="seriesSet" uid="..."/>
           <out group="series">
               <key index="0" uid="..."/>
               <key index="3" uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection Input Input
 * - \b seriesSet [sight::data::series_set]: database series from which the series extracted.
 *
 * @subsection Output Output
 * - \b series [sight::data::series]: output series extracted from the database series.
 */
class get_series : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(get_series, service::controller);

    /// Constructor
    get_series() = default;

    /// Destructor
    ~get_series() override = default;

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
    static constexpr std::string_view SERIES_SET_INPUT = "seriesSet";
    static constexpr std::string_view SERIES_GROUP_OUT = "series";
    sight::data::ptr<sight::data::series_set, sight::data::access::in> m_series_set {this, SERIES_SET_INPUT};
    sight::data::ptr_vector<sight::data::series, sight::data::access::out> m_series {this, SERIES_GROUP_OUT};
};

} // namespace sight::module::data
