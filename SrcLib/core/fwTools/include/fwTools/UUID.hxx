/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*
 * UUID.hxx
 *
 *  Created on: Aug 19, 2008
 *      Author: vagnus
 */

namespace fwTools
{

//template<class PTR>
//std::string UUID::get(PTR *ptr)
//{
//	for (UUIDContainer::iterator i = m_uuids.begin(); i != m_uuids.end(); ++i)
//	{
//		if (!i->first.expired() && i->first.lock().get() == ptr)
//		{
//			return i->second;
//		}
//	}
//
//	CANNOT GENERATE AND STORE THE KEY !!! THROW AN EXCEPTION ????
//}




template<class T>
std::string UUID::get(boost::shared_ptr<T> sptr,const int &version)
{
	for (UUIDContainer::iterator i = begin(m_uuids); i != m_uuids.end();  next(i) )
	{
		if ( i->first.lock().get() == sptr.get() )
		{
			return translateUUID( i->second,version);
		}
	}
	// no one found generate a new one
	MultiUUID newUUID =  UUID::generate(typeid( *sptr.get() )); // typeid(*ptr) is doesn't take into account const version :)
	m_uuids[sptr] = newUUID;
	return translateUUID(newUUID,version);
}




template<class T>
std::string UUID::get(boost::weak_ptr<T> wptr, const int &version)
{
	assert(wptr.lock().get());
	return get( wptr.lock(), version );
}



template<class T>
boost::shared_ptr<T> UUID::get( const std::string & uuid,const int &version)
{
	UUIDContainer::iterator i;
	OSLM_TRACE( "UUID::get("<< uuid << "v=" << version <<")");
	i = begin(m_uuids);
	while ( i != m_uuids.end() )
	{
		//std::cout<< "****" << translateUUID( i->second, version) << std::endl;
		OSLM_TRACE( "UUID::get translateUUID =("<< translateUUID( i->second, version) );
		if ( translateUUID( i->second, version) == uuid )
		{
			boost::shared_ptr<T> obj = ::boost::static_pointer_cast<T>( i->first.lock() );
			OSLM_TRACE( "UUID::get with uuid=" << uuid << " Object found");
			return obj;
		}
		next(i);
	}
	OSLM_TRACE( "UUID::get with uuid=" << uuid << " Object NOT found !!!");


	return boost::shared_ptr<T>();
}



template<class T>
bool UUID::supervise(T *ptr)
{
	for (UUIDContainer::iterator i = begin(m_uuids); i != m_uuids.end();  next(i) )
	{
		if ( i->first.lock().get() == ptr )
		{
			return true;
		}
	}
	return false;
}

template<class T>
bool UUID::supervise(boost::shared_ptr<T> sptr)
{
	return supervise(sptr.get());
}


template<class T>
bool UUID::supervise(boost::weak_ptr<T> wptr)
{
	assert(wptr.lock());
	return supervise(wptr.lock().get());
}



template<class T>
void UUID::impose( boost::shared_ptr<T> sptr, const std::string &givenUUID, const int version) throw (std::invalid_argument)
{
	assert( sptr && !givenUUID.empty() );
	if ( exist(givenUUID,version))
	{
		throw std::invalid_argument( std::string("UUID::impose failed uuid [") + givenUUID + "] already exist");
	}
	if ( supervise(sptr) )
	{
		std::string msg("UUID::impose failed object have a previous uuid");
		msg += " old=" + UUID::get(sptr) + " imposed=" + givenUUID;
		throw std::invalid_argument( msg );
	}

	boost::weak_ptr<void> wp(sptr);
	UUID::MultiUUID uuid = generate(typeid(T));
	typename UUID::UUIDContainer::value_type newEntry( wp , uuid );

//	newEntry.first.swap(wp);
//	newEntry.second = generate(typeid(T) );

	if ( version == UUID::SIMPLE )
	{
		newEntry.second.first = givenUUID;
	}
	else if ( version == UUID::EXTENDED )
	{
		newEntry.second.second = givenUUID;
	}
	else
	{
		throw std::invalid_argument( "UUID::impose not managed version" );
	}
	m_uuids.insert(newEntry);

	OSLM_DEBUG("UUID::impose sptr.get()"<<  sptr.get() << " suuid=" << m_uuids[sptr].first << " euuid" << m_uuids[sptr].second << " givenuuid=" <<givenUUID );

}



template<class T>
void UUID::impose( boost::weak_ptr<T> wptr, const std::string &uuid,const int version) throw (std::invalid_argument)
{
	assert( wptr );
	impose( wptr.lock() ,uuid , version );
}



} // end namespace fwTools

