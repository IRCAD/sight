/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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
    const auto config_out    = config.equal_range("out");
    for(auto it = config_out.first ; it != config_out.second ; ++it)
    {
        const service::config_t& attr = it->second.get_child("<xmlattr>.group");

        if(attr.get_value<std::string>() == "camera")
        {
            const auto key_range = it->second.equal_range("key");
            for(auto it_key = key_range.first ; it_key != key_range.second ; ++it_key)
            {
                auto index = it_key->second.get<size_t>("<xmlattr>.index", 0);

                m_cameraIndexNumbers.push_back(index);
            }
        }
        else if(attr.get_value<std::string>() == "extrinsic")
        {
            const auto key_range = it->second.equal_range("key");
            for(auto it_key = key_range.first ; it_key != key_range.second ; ++it_key)
            {
                auto index = it_key->second.get<size_t>("<xmlattr>.index", 0);
                m_extrinsicIndexNumbers.push_back(index);
            }
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
    for(auto& index : m_cameraIndexNumbers)
    {
        m_camera[i] = camera_set->get_camera(index);
        i++;
    }

    if(!m_extrinsicIndexNumbers.empty())
    {
        size_t j = 0;
        for(auto& index : m_extrinsicIndexNumbers)
        {
            if(camera_set->get_extrinsic_matrix(index) == nullptr)
            {
                SIGHT_THROW_EXCEPTION(sight::data::exception("Cameras does not have extrinsic Matrix"));
            }

            m_extrinsic[j] = camera_set->get_extrinsic_matrix(index);
            j++;
        }
    }
}

//-----------------------------------------------------------------------------

void get_camera::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
