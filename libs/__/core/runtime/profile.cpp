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

#include "core/runtime/detail/profile/profile.hpp"
#include "core/runtime/profile.hpp"

#include "core/runtime/detail/profile/activater.hpp"
#include "core/runtime/extension.hpp"
#include "core/runtime/module.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace sight::core::runtime
{

namespace
{

template<typename E>
struct apply
{
    //------------------------------------------------------------------------------

    void operator()(E _e)
    {
        _e->apply();
    }
};

} // namespace

//------------------------------------------------------------------------------

profile::~profile()
{
    delete[] m_argv;
}

//------------------------------------------------------------------------------

void profile::set_params(const profile::params_container& _params)
{
    m_params = _params;

    delete[] m_argv;

    m_argc = static_cast<int>(m_params.size());
    // allocate memory for an array of character strings
    m_argv = new char*[m_params.size()];

    // for each string, allocate memory in the character array and copy
    for(std::size_t i = 0 ; i < m_params.size() ; i++)
    {
        std::size_t param_size = m_params[i].size();
        m_argv[i] = new char [param_size + 1];
#ifndef _WIN32
        strncpy(m_argv[i], m_params[i].c_str(), param_size);
        m_argv[i][param_size] = '\0';
#else
        strncpy_s(m_argv[i], param_size + 1, m_params[i].c_str(), param_size);
#endif
    }
}

//------------------------------------------------------------------------------

profile::sptr get_current_profile()
{
    return detail::profile::get_current_profile();
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
