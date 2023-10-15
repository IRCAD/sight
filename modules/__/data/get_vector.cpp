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

#include "get_vector.hpp"

namespace sight::module::data
{

//------------------------------------------------------------------------------

void get_vector::configuring()
{
    service::config_t config = this->get_config();
    const config_t configOut = config.get_child("out");

    const auto keyCfg = configOut.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::config_t& attr = itCfg->second.get_child("<xmlattr>.index");
        const auto index              = attr.get_value<size_t>();
        m_indexNumbers.push_back(index);
    }
}

//------------------------------------------------------------------------------

void get_vector::starting()
{
}

//-----------------------------------------------------------------------------

void get_vector::updating()
{
    const auto vector = m_vector.lock();
    if(vector == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing input vector"));
    }

    size_t i = 0;
    for(const auto& index : m_indexNumbers)
    {
        if(index < vector->size())
        {
            m_objects[i] = vector->at(index);
        }
        else
        {
            m_objects[i] = nullptr;
        }

        i++;
    }
}

//-----------------------------------------------------------------------------

void get_vector::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
