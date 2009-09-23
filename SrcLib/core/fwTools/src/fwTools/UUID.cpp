/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/Demangler.hpp>

#ifdef WIN32
  #include <RPC.h> // use Rpcrt4.dll
#else
  #include <uuid/uuid.h> // in package uuid-dev and used libuuid
#endif

#include "fwTools/UUID.hpp"


namespace fwTools
{
std::map< TypeInfo, ::boost::uint32_t > UUID::m_CategorizedCounter;
UUID::UUIDContainer UUID::m_uuids;


UUID::UUID()
{
}

UUID::~UUID()
{
}


void UUID::next( UUIDContainer::iterator &iter )
{
	UUIDContainer::iterator iterCurrent = iter;
        if ( iterCurrent != m_uuids.end() ) // On n'est pas à la fin ...
	{
		iter++; // On passe au suivant
		if ( iterCurrent->first.expired() == true)
		{
			// le précédent est expiré, on l'efface ...
			m_uuids.erase(iterCurrent); // OK C++ stdLib book p 205
		}

		if ( iter != m_uuids.end() && iter->first.expired() == true ) // si le suivant est différent de fin
		{
			next( iter ) ; // if not expired we do not need to search next
		}
	}
}


UUID::UUIDContainer::iterator UUID::begin(UUIDContainer &uuidContainer)
{
	 UUIDContainer::iterator i=m_uuids.begin();
	 while( i != m_uuids.end() && i->first.expired() )
	 {
		 next(i);
	 }
     return i;
}




bool UUID::exist( const std::string & uuid,const int &version)
{
	UUIDContainer::iterator i= begin(m_uuids) ;
	while ( i!= m_uuids.end() )
	{
		if ( translateUUID(i->second,version) == uuid)
		{
			return true;
		}
		next(i);
	}
	return false;
}




std::string UUID::generateExtendedUUID()
{
	std::string extUUID;

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



UUID::MultiUUID UUID::generate(const std::type_info &ti)
{
	//typedef BaseType
	MultiUUID newUUID;
	std::string prefix = ::fwCore::Demangler(ti).getRootedClassname();
	do
	{
		//std::cout << "ti.name()"
		newUUID.first = prefix  + "-" + boost::lexical_cast<std::string>( m_CategorizedCounter[ti]++ );
	}
	while ( exist(newUUID.first,UUID::SIMPLE) ); // necessary due to manual set

	newUUID.second = generateExtendedUUID();

	return newUUID;
}


std::string UUID::translateUUID(const MultiUUID &muuid, const int &version)
{
	// OSLM_DEBUG("UUID::translateUUID Vsimple=" << muuid.first << " vExtended=" << muuid.second );
	assert( version == UUID::SIMPLE || version == UUID::RFC4122 );
	if ( version ==  UUID::SIMPLE )
	{
		return muuid.first;
	}
	else if ( version == UUID::RFC4122 )
	{
		return muuid.second;
	}
	return "UUID::version not recognized";
}


// helper to find a weak_ptr in the m_uuids
UUID::UUIDContainer::iterator UUID::find( boost::weak_ptr<void> wp )
{
	assert ( !wp.expired() );

	boost::shared_ptr<void> sp = wp.lock();
	UUIDContainer::iterator i= begin( m_uuids) ;
	while ( i!= m_uuids.end() )
	{

		if ( i->first.expired()==false   &&    sp == i->first.lock()  )
		{
				return i;
		}
		next(i);
	}
	return m_uuids.end();
}



}
