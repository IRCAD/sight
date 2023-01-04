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

#include "data/ptr.hpp"

namespace sight::data
{

//------------------------------------------------------------------------------

base_ptr::base_ptr(
    data::IHasData* _holder,
    std::string_view _key,
    bool _autoConnect,
    bool _optional,
    Access _access,
    std::optional<std::size_t> _index
) :
    m_holder(_holder),
    m_key(_key),
    m_autoConnect(_autoConnect),
    m_optional(_optional),
    m_access(_access)
{
    _holder->registerPtr(_key, this, _index);
}

//------------------------------------------------------------------------------

base_ptr::~base_ptr()
{
    m_holder->unregisterPtr(this);
}

//------------------------------------------------------------------------------

} // namespace sight::data
