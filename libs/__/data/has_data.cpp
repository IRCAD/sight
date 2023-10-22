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

#include "data/has_data.hpp"

#include <core/com/signal.hxx>

#include <data/ptr.hpp>

namespace sight::data
{

//------------------------------------------------------------------------------

data::object::csptr has_data::object(
    std::string_view _key,
    data::Access _access,
    std::optional<std::size_t> _index
) const
{
    if(auto it_data = m_dataContainer.find({_key, _index}); it_data != m_dataContainer.end())
    {
        const auto& d = it_data->second;
        if(d->access() == _access)
        {
            return d->get();
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void has_data::set_input(
    data::object::csptr _obj,
    std::string_view _key,
    std::optional<bool> _auto_connect,
    std::optional<bool> _optional,
    std::optional<std::size_t> _index
)
{
    auto data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT("Could not find any registered data pointer with key '" << _key << "'", data != m_dataContainer.end());
    SIGHT_ASSERT("Key '" << _key << "' is not an input.", data->second->access() == data::Access::INPUT);
    data->second->set(std::const_pointer_cast<sight::data::object>(_obj), _auto_connect, _optional, _index);
}

//------------------------------------------------------------------------------

void has_data::set_inout(
    data::object::sptr _obj,
    std::string_view _key,
    std::optional<bool> _auto_connect,
    std::optional<bool> _optional,
    std::optional<std::size_t> _index
)
{
    auto data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT("Could not find any registered data pointer with key '" << _key << "'", data != m_dataContainer.end());
    SIGHT_ASSERT("Key '" << _key << "' is not an in/out.", data->second->access() == data::Access::INOUT);
    data->second->set(_obj, _auto_connect, _optional, _index);
}

//-----------------------------------------------------------------------------

void has_data::set_output(std::string_view _key, data::object::sptr _obj, std::optional<std::size_t> _index)
{
    auto data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT("Could not find any registered data pointer with key '" << _key << "'", data != m_dataContainer.end());
    SIGHT_ASSERT("Key '" << _key << "' is not an output.", data->second->access() == data::Access::OUTPUT);
    data->second->set(_obj, {}, {}, _index, true);
}

//------------------------------------------------------------------------------

void has_data::set_object(
    data::object::sptr _obj,
    std::string_view _key,
    std::optional<std::size_t> _index,
    [[maybe_unused]] data::Access _access,
    const bool _auto_connect,
    const bool _optional
)
{
    auto data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT("Could not find any registered data pointer with key '" << _key << "'", data != m_dataContainer.end());
    SIGHT_ASSERT("Key '" << _key << "' is not an in/out.", data->second->access() == _access);
    data->second->set(_obj, _auto_connect, _optional, _index);
}

//------------------------------------------------------------------------------

void has_data::reset_object(std::string_view _key, std::optional<std::size_t> _index)
{
    auto data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT("Could not find any registered data pointer with key '" << _key << "'", data != m_dataContainer.end());
    data->second->set(nullptr, {}, {}, _index);
}

//-----------------------------------------------------------------------------

void has_data::set_deferred_id(std::string_view _key, const std::string& _id, std::optional<std::size_t> _index)
{
    auto it_data = m_dataContainer.find({_key, {}});
    SIGHT_ASSERT(
        "Could not find any registered data pointer with key '" << _key << "'",
        it_data != m_dataContainer.end()
    );
    it_data->second->set_deferred_id(_id, _index);
}

//------------------------------------------------------------------------------

void has_data::registerPtr(std::string_view _key, base_ptr* _data, std::optional<std::size_t> _index)
{
    m_dataContainer[{_key, _index}] = _data;
}

//------------------------------------------------------------------------------

void has_data::unregisterPtr(base_ptr* _data)
{
    std::erase_if(m_dataContainer, [_data](const auto& _p){return _p.second == _data;});
}

//------------------------------------------------------------------------------

const has_data::container_t& has_data::container() const
{
    return m_dataContainer;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
