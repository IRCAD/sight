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

#include <core/base.hpp>

#include <data/SeriesDB.hpp>

#include <service/IController.hpp>
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
        <service uid="..." type="sight::module::data::SGetSeries" >
           <in key="seriesDB" uid="..."/>
           <out group="series">
               <key index="0" uid="..."/>
               <key index="3" uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection Input Input
 * - \b seriesDB [sight::data::SeriesDB]: database series from which the series extracted.
 *
 * @subsection Output Output
 * - \b series [sight::data::Series]: output series extracted from the database series.
 */
class MODULE_DATA_CLASS_API SGetSeries : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SGetSeries, service::IController);

    /// Constructor
    MODULE_DATA_API SGetSeries() = default;

    /// Destructor
    MODULE_DATA_API ~SGetSeries() = default;

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
    static constexpr std::string_view s_SERIESDB_INPUT   = "seriesDB";
    static constexpr std::string_view s_SERIES_GROUP_OUT = "series";
    sight::data::ptr<sight::data::SeriesDB, sight::data::Access::in> m_seriesDB {this, s_SERIESDB_INPUT};
    sight::data::ptr_vector<sight::data::Series, sight::data::Access::out> m_series {this, s_SERIES_GROUP_OUT};
};

} // namespace sight::module::data
