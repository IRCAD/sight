/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

// cspell:ignore NOLINT

#include "object.hpp"

#include "tools/uuid.hpp"

#include <map>
#include <mutex>

namespace sight::core
{

static std::map<const object*, std::string> s_ptr_to_uuid;
static std::map<std::string, object::wptr> s_uuid_to_wptr;
static std::mutex s_maps_mutex;

//------------------------------------------------------------------------------

object::sptr object::from_uuid(const std::string& _uuid)
{
    // Find the weak pointer associated to this uuid
    std::lock_guard guard(s_maps_mutex);
    const auto found = s_uuid_to_wptr.find(_uuid);

    if(found != s_uuid_to_wptr.cend())
    {
        // If we found it, return it now
        return found->second.lock();
    }

    return {};
}

//------------------------------------------------------------------------------

std::string object::get_uuid() const
{
    // First, try to find the uuid
    std::lock_guard guard(s_maps_mutex);
    const auto found = s_ptr_to_uuid.find(this);

    if(found != s_ptr_to_uuid.cend())
    {
        // If we found it, return it now
        return found->second;
    }

    // If not found, generate a new one
    std::string uuid;

    // Since we can manually set an uuid, we must ensure that the generated one did not generate an existing one.
    // Almost impossible, but possible
    do
    {
        uuid = tools::uuid::generate();
    }
    while(s_uuid_to_wptr.find(uuid) != s_uuid_to_wptr.end());

    // Store the uuid
    s_ptr_to_uuid[this] = uuid;

    // Store also a weak pointer
    // As we don't modify the underlying object, but only store a non const weak pointer,
    // the "const" contract is not really broken in this method.
    // This is a compromise to allow a "lazy" uuid generation
    s_uuid_to_wptr[uuid] = const_cast<object*>(this)->get_sptr(); // NOLINT(cppcoreguidelines-pro-type-const-cast)

    return uuid;
}

//------------------------------------------------------------------------------

void object::set_uuid(const std::string& _uuid, const bool _force)
{
    std::lock_guard guard(s_maps_mutex);

    if(!_force)
    {
        // Look if the uuid has already been used
        const auto found_wptr = s_uuid_to_wptr.find(_uuid);
        if(found_wptr != s_uuid_to_wptr.cend())
        {
            // Try to see if the object is different
            object::sptr old = found_wptr->second.lock();

            if(old && this != old.get())
            {
                /// That's no good, we set an already used uuid
                SIGHT_FATAL("UUID '" << _uuid << "' is already used for object '" << old->get_id() << "'");
            }
        }

        // Look if the object has already been registered
        const auto found_uuid = s_ptr_to_uuid.find(this);
        if(found_uuid != s_ptr_to_uuid.cend())
        {
            // Try to see if the uuid is different
            const std::string& old = found_uuid->second;
            if(!old.empty() && _uuid != old)
            {
                /// That's no good, the object has already be registered with a different uuid
                SIGHT_FATAL("Object '" << get_id() << "' is already registered with UUID '" << old << "'");
            }
        }
    }

    s_ptr_to_uuid[this]   = _uuid;
    s_uuid_to_wptr[_uuid] = get_sptr();
}

object::~object()
{
    std::lock_guard guard(s_maps_mutex);
    const auto found = s_ptr_to_uuid.find(this);

    // If we found it, erase us.
    if(found != s_ptr_to_uuid.cend())
    {
        s_uuid_to_wptr.erase(found->second);
        s_ptr_to_uuid.erase(found);
    }
}

} //namespace sight::core
