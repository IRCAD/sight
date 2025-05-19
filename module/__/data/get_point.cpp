/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "get_point.hpp"

#include <data/fiducials_series.hpp>

namespace sight::module::data
{

//------------------------------------------------------------------------------

void get_point::configuring()
{
    service::config_t config = this->get_config();
    const auto config_out    = config.get_child_optional("inout");
    SIGHT_THROW_IF("No inout configured, no point to extract", not config_out.has_value());
    const auto key_cfg = config_out->equal_range("key");

    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        auto group_cfg = it_cfg->second.get<std::string>("<xmlattr>.group", "");
        SIGHT_ASSERT("Missing 'group' attribute to extract point.", !group_cfg.empty());
        auto index_cfg = it_cfg->second.get<std::size_t>("<xmlattr>.index", ~0UL);
        SIGHT_ASSERT("Missing 'index' attribute to extract point", index_cfg != ~0UL);

        m_configuration.push_back({.group = group_cfg, .index = index_cfg});
    }

    SIGHT_THROW_IF("Configuration empty, no point to extract", m_configuration.empty());
}

//------------------------------------------------------------------------------

void get_point::starting()
{
}

//-----------------------------------------------------------------------------

void get_point::updating()
{
    auto image = m_image.lock();
    SIGHT_ASSERT("Missing input image", image);

    auto fiducials = image->get_fiducials();

    std::size_t out_index = 0;
    for(const auto& cfg : m_configuration)
    {
        const auto results = fiducials->query_fiducials(
            std::nullopt,
            sight::data::fiducials_series::shape::point,
            cfg.group
        );
        SIGHT_THROW_IF("Can not find any group named " << std::quoted(cfg.group), results.empty());
        SIGHT_THROW_IF("Index out of bounds", cfg.index >= results.size());

        const auto& result = results[cfg.index];
        SIGHT_THROW_IF("No contour data for this fiducial", not result.m_contour_data.has_value());
        SIGHT_THROW_IF("Contour data does not have exactly 3 coordinates", result.m_contour_data->size() != 3);

        auto point = m_points[out_index++].lock();
        (*point)[0] = (*result.m_contour_data)[0];
        (*point)[1] = (*result.m_contour_data)[1];
        (*point)[2] = (*result.m_contour_data)[2];

        point->async_emit(sight::data::object::MODIFIED_SIG);
    }
}

//-----------------------------------------------------------------------------

void get_point::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
