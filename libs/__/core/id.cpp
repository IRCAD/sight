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

// cspell:ignore NOLINTNEXTLINE

#include "id.hpp"

#include "demangler.hpp"
#include "object.hpp"

#include "tools/failed.hpp"

#include <boost/lexical_cast.hpp>

namespace sight::core
{

id::categorized_counter id::s_categorized_counter;
id::dictionary id::s_dictionary;
core::mt::read_write_mutex id::s_dictionary_mutex;
core::mt::mutex id::s_mutex_counter;

//-----------------------------------------------------------------------------

id::~id()
{
    reset_id();
}

//-----------------------------------------------------------------------------

bool id::exist(type _id)
{
    core::mt::read_lock lock(s_dictionary_mutex);
    return id::is_id_found(_id);
}

//-----------------------------------------------------------------------------

bool id::is_id_found(type _id)
{
    return s_dictionary.contains(_id);
}

//-----------------------------------------------------------------------------

bool id::has_id() const
{
    core::mt::read_lock lock(m_id_mutex);
    return !m_id.empty();
}

//-----------------------------------------------------------------------------

void id::set_id(type _new_id)
{
    core::mt::write_lock lock(m_id_mutex);
    this->add_id_in_dictionary(_new_id);
}

//-----------------------------------------------------------------------------

void id::add_id_in_dictionary(type _new_id) const
{
    core::mt::write_lock lock(s_dictionary_mutex);

    SIGHT_FATAL_IF("Try to set an existing fwID = " << _new_id, is_id_found(_new_id));

    id::remove_id_from_dictionary(m_id);
    // note we use a static cast for a down cast because we do not use the classical polymorphic approach
    //m_dictionary[ newID ] = (static_cast< Object *>(this))->get_sptr();
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    s_dictionary[_new_id] = ((object*) (this))->get_sptr();
    m_id                  = _new_id;
}

//-----------------------------------------------------------------------------

id::type id::get_id(policy _policy) const
{
    core::mt::read_to_write_lock lock(m_id_mutex);
    if(m_id.empty()) // no id set
    {
        if(_policy == policy::generate)
        {
            type new_id = generate();
            core::mt::upgrade_to_write_lock write_lock(lock);
            this->add_id_in_dictionary(new_id);
        }
        else if(_policy == policy::empty)
        {
            /* nothing to do*/
        }
        else if(_policy == policy::must_exist)
        {
            throw core::tools::failed("id::get_id() no id set");
        }
    }

    return m_id;
}

//------------------------------------------------------------------------------

id::type id::base_id(policy _policy) const
{
    const auto& id = get_id(_policy);

    if(const auto pos = id.find_last_of(s_separator); pos != std::string::npos)
    {
        return id.substr(pos + 1);
    }

    return id;
}

//-----------------------------------------------------------------------------

id::type id::generate() const
{
    type new_id;
    std::string prefix = this->get_classname();
    do
    {
        core::mt::scoped_lock lock(s_mutex_counter);
        new_id = prefix + "-" + std::to_string(s_categorized_counter[prefix]++);
    }
    while(exist(new_id));

    return new_id;
}

//-----------------------------------------------------------------------------

object::sptr id::get_object(id::type _request_id)
{
    core::mt::read_lock lock(s_dictionary_mutex);
    auto it = s_dictionary.find(_request_id);
    if(it != s_dictionary.end())
    {
        SIGHT_ASSERT("expired object in fwID::Dictionary for id=" + _request_id, !it->second.expired());
        return it->second.lock();
    }

    return {};
}

//-----------------------------------------------------------------------------

void id::reset_id()
{
    core::mt::write_lock lock(m_id_mutex);
    core::mt::write_lock dico_lock(s_dictionary_mutex);
    id::remove_id_from_dictionary(m_id);
    m_id.clear();
}

//-----------------------------------------------------------------------------

void id::remove_id_from_dictionary(type _id)
{
    if(!_id.empty())
    {
        s_dictionary.erase(_id);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core
