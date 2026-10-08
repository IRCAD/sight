/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "get_camera.hpp"

namespace sight::module::data
{

//------------------------------------------------------------------------------

void get_camera::configuring()
{
    service::config_t config = this->get_config();
    const auto config_output = config.equal_range("output");
    for(auto it = config_output.first ; it != config_output.second ; ++it)
    {
        const auto camera = it->second.get_child_optional("<xmlattr>.index");
        const auto index  = camera ? camera->get_value<size_t>() : 0;
        const auto kind   = it->second.get_optional<std::string>("<xmlattr>.kind");

        if(it->second.get_child_optional("<xmlattr>.camera") || (kind && *kind == "camera"))
        {
            m_camera_index_numbers.push_back(index);
        }

        if(it->second.get_child_optional("<xmlattr>.extrinsic") || (kind && *kind == "extrinsic"))
        {
            m_extrinsic_index_numbers.push_back(index);
        }
    }
}

//------------------------------------------------------------------------------

void get_camera::starting()
{
}

//-----------------------------------------------------------------------------

void get_camera::updating()
{
    const auto camera_set = m_camera_set.lock();

    if(camera_set == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing input camera series"));
    }

    size_t i = 0;
    for(auto& index : m_camera_index_numbers)
    {
        m_camera[i] = camera_set->get_camera(index);
        i++;
    }

    if(!m_extrinsic_index_numbers.empty())
    {
        for(auto& index : m_extrinsic_index_numbers)
        {
            if(camera_set->get_extrinsic_matrix(index) == nullptr)
            {
                SIGHT_THROW_EXCEPTION(sight::data::exception("Cameras does not have extrinsic Matrix"));
            }

            m_extrinsic[index] = camera_set->get_extrinsic_matrix(index);
        }
    }
}

//-----------------------------------------------------------------------------

void get_camera::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
