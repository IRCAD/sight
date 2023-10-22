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

#include "get_series.hpp"

namespace sight::module::data
{

//------------------------------------------------------------------------------

void get_series::configuring()
{
    service::config_t config  = this->get_config();
    const config_t config_out = config.get_child("out");

    const auto key_cfg = config_out.equal_range("key");
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        const service::config_t& attr = it_cfg->second.get_child("<xmlattr>.index");
        const auto index              = attr.get_value<size_t>();
        m_indexNumbers.push_back(index);
    }
}

//------------------------------------------------------------------------------

void get_series::starting()
{
}

//-----------------------------------------------------------------------------

void get_series::updating()
{
    const auto series_set = m_series_set.lock();
    if(series_set == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing input database series"));
    }

    if(series_set->empty())
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("The database series is empty, nothing can be extracted."));
    }

    size_t i = 0;
    for(const auto& index : m_indexNumbers)
    {
        m_series[i] = series_set->at(index);
        i++;
    }
}

//-----------------------------------------------------------------------------

void get_series::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
