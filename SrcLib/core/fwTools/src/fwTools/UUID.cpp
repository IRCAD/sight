/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "fwTools/UUID.hpp"

namespace fwTools
{

UUID::UUIDContainer UUID::s_uuidMap;

::fwCore::mt::ReadWriteMutex UUID::s_rwMutex;
::fwCore::mt::Mutex UUID::s_mutex;

//-----------------------------------------------------------------------------

UUID::UUID() : m_uuid("")
{}

//-----------------------------------------------------------------------------

UUID::~UUID()
{
    UUID::UUIDContainer::iterator iter = UUID::s_uuidMap.find(m_uuid);
    if( iter != UUID::s_uuidMap.end())
    {
        UUID::s_uuidMap.erase(iter);
    }
}

//-----------------------------------------------------------------------------

bool UUID::exist( const UUID::UUIDType & uuid)
{
    ::fwCore::mt::ReadLock lock(s_rwMutex);
    return ( UUID::s_uuidMap.find(uuid) != UUID::s_uuidMap.end() );
}

//-----------------------------------------------------------------------------

const UUID::UUIDType& UUID::get(::fwTools::Object::sptr object)
{
    SLM_ASSERT("Object expired", object);

    ::fwCore::mt::ReadToWriteLock lock(s_rwMutex);
    UUID::sptr uuidObject = object->m_uuid;
    if(uuidObject->m_uuid.empty())
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        uuidObject->m_uuid = UUID::generateUUID();
        UUID::s_uuidMap.insert(UUID::UUIDContainer::value_type(uuidObject->m_uuid, object));
    }
    return uuidObject->m_uuid;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr UUID::get( const UUID::UUIDType & uuid )
{
    ::fwCore::mt::ReadLock lock(s_rwMutex);
    ::fwTools::Object::sptr obj;
    UUID::UUIDContainer::iterator iter = UUID::s_uuidMap.find(uuid);
    if( iter != UUID::s_uuidMap.end() )
    {
        obj = iter->second.lock();
    }
    return obj;
}

//-----------------------------------------------------------------------------

bool UUID::set(::fwTools::Object::sptr object, const UUID::UUIDType & uuid )
{
    bool setted = false;

    ::fwCore::mt::ReadToWriteLock lock(s_rwMutex);

    if(!UUID::exist(uuid))
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        UUID::sptr uuidObject = object->m_uuid;
        uuidObject->m_uuid = uuid;
        UUID::s_uuidMap.insert(UUID::UUIDContainer::value_type(uuidObject->m_uuid, object));
        setted = true;
    }

    return setted;
}

//-----------------------------------------------------------------------------

UUID::UUIDType UUID::generateUUID()
{
    ::fwCore::mt::ScopedLock lock(s_mutex);
    static boost::uuids::random_generator gen;
    ::boost::uuids::uuid uuid = gen();
    return ::boost::uuids::to_string(uuid);
}

//-----------------------------------------------------------------------------

}
