/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwTools/UUID.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace fwTools
{

UUID::UUIDContainer UUID::s_uuidMap;

::fwCore::mt::ReadWriteMutex UUID::s_uuidMapMutex;
::fwCore::mt::Mutex UUID::s_generateUUIDMutex;

//-----------------------------------------------------------------------------

UUID::UUID()
{
}

//-----------------------------------------------------------------------------

UUID::~UUID()
{
    ::fwCore::mt::ReadLock uuidLock(m_uuidMutex);
    ::fwCore::mt::ReadToWriteLock lock(s_uuidMapMutex);
    UUID::UUIDContainer::iterator iter = UUID::s_uuidMap.find(m_uuid);
    if( iter != UUID::s_uuidMap.end())
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        UUID::s_uuidMap.erase(iter);
    }
}

//-----------------------------------------------------------------------------

bool UUID::exist( const UUID::UUIDType& uuid)
{
    ::fwCore::mt::ReadLock lock(s_uuidMapMutex);
    return ( UUID::s_uuidMap.find(uuid) != UUID::s_uuidMap.end() );
}

//-----------------------------------------------------------------------------

const UUID::UUIDType& UUID::get(::fwTools::Object::sptr object)
{
    SLM_ASSERT("Object expired", object);

    UUID::sptr uuidObject = object->m_uuid;
    ::fwCore::mt::ReadToWriteLock uuidLock(uuidObject->m_uuidMutex);
    if(uuidObject->m_uuid.empty())
    {
        UUIDType uuid = UUID::generateUUID();

        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(uuidLock);
            uuidObject->m_uuid = uuid;
        }
        {
            ::fwCore::mt::WriteLock lock(s_uuidMapMutex);
            UUID::s_uuidMap.insert(UUID::UUIDContainer::value_type(uuid, object));
        }
    }
    return uuidObject->m_uuid;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr UUID::get( const UUID::UUIDType& uuid )
{
    ::fwCore::mt::ReadLock lock(s_uuidMapMutex);
    ::fwTools::Object::sptr obj;
    UUID::UUIDContainer::iterator iter = UUID::s_uuidMap.find(uuid);
    if( iter != UUID::s_uuidMap.end() )
    {
        obj = iter->second.lock();
    }
    return obj;
}

//-----------------------------------------------------------------------------

bool UUID::set(::fwTools::Object::sptr object, const UUID::UUIDType& uuid )
{
    ::fwCore::mt::ReadToWriteLock lock(s_uuidMapMutex);

    bool isSet = false;

    if(UUID::s_uuidMap.find(uuid) == UUID::s_uuidMap.end())
    {
        UUID::sptr uuidObject = object->m_uuid;

        {
            ::fwCore::mt::WriteLock uuidLock(uuidObject->m_uuidMutex);
            uuidObject->m_uuid = uuid;
        }
        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            UUID::s_uuidMap.insert(UUID::UUIDContainer::value_type(uuid, object));
        }
        isSet = true;
    }

    return isSet;
}

//-----------------------------------------------------------------------------

UUID::UUIDType UUID::generateUUID()
{
    static ::boost::uuids::random_generator gen;
    ::fwCore::mt::ScopedLock lock(s_generateUUIDMutex);
    ::boost::uuids::uuid uuid = gen();
    return ::boost::uuids::to_string(uuid);
}

//-----------------------------------------------------------------------------

}
