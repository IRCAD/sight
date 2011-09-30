/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifdef WIN32
  #include <RPC.h> // use Rpcrt4.dll
#else
  #include <uuid/uuid.h> // in package uuid-dev and used libuuid
#endif

#include "fwTools/UUID.hpp"

namespace fwTools
{

UUID::UUIDContainer UUID::m_uuidMap;

//-----------------------------------------------------------------------------

UUID::UUID() : m_uuid("")
{}

//-----------------------------------------------------------------------------

UUID::~UUID()
{
    UUID::UUIDContainer::iterator iter = UUID::m_uuidMap.find(m_uuid);
    if( iter != UUID::m_uuidMap.end())
    {
        UUID::m_uuidMap.erase(iter);
    }
}

//-----------------------------------------------------------------------------

bool UUID::exist( const UUID::UUIDType & uuid)
{
    return ( UUID::m_uuidMap.find(uuid) != UUID::m_uuidMap.end() );
}

//-----------------------------------------------------------------------------

const UUID::UUIDType& UUID::get(::fwTools::Object::sptr object)
{
    SLM_ASSERT("Object expired", !object);
    UUID::sptr uuidObject = object->m_uuid;
    if(uuidObject->m_uuid.empty())
    {
        uuidObject->m_uuid = UUID::generateUUID();
        UUID::m_uuidMap.insert(UUID::UUIDContainer::value_type(uuidObject->m_uuid, object));
    }
    return uuidObject->m_uuid;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr UUID::get( const UUID::UUIDType & uuid )
{
    ::fwTools::Object::sptr obj;
    UUID::UUIDContainer::iterator iter = UUID::m_uuidMap.find(uuid);
    if( iter != UUID::m_uuidMap.end() )
    {
        obj = iter->second.lock();
    }
    return obj;
}

//-----------------------------------------------------------------------------

UUID::UUIDType UUID::generateUUID()
{
    UUID::UUIDType extUUID;

#ifdef WIN32
        UCHAR *str = NULL;
        GUID guid;
        UuidCreate(&guid);
        UuidToString(&guid,&str);
        extUUID = std::string((char *)(str));
        RpcStringFree(&str);
#else
        uuid_t id;
        uuid_generate_random( id );
        char str[255];
        uuid_unparse( id, str );
        extUUID = std::string(str);
#endif
        return extUUID;
}

//-----------------------------------------------------------------------------

}
