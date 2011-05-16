/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*
 * UUID.hxx
 *
 *  Created on: Aug 19, 2008
 * @author  IRCAD (Research and Development Team).
  */

namespace fwTools
{

template<class T>
std::string UUID::get(boost::shared_ptr<T> sptr )
{
    for (UUIDContainer::iterator i = begin(m_uuids); i != m_uuids.end();  next(i) )
    {
        if ( i->first.lock().get() == sptr.get() )
        {
            return   i->second ;
        }
    }
    // no one found generate a new one
    UUIDType newUUID =  generateUUID();
    m_uuids[sptr] = newUUID;
    return newUUID;
}

template<class T>
std::string UUID::get(boost::weak_ptr<T> wptr)
{
    assert(wptr.lock().get());
    return get( wptr.lock() );
}

template<class T>
boost::shared_ptr<T> UUID::get( const std::string & uuid)
{
    UUIDContainer::iterator i;
    OSLM_TRACE( "UUID::get("<< uuid << ")");
    i = begin(m_uuids);
    while ( i != m_uuids.end() )
    {
        if ( i->second  == uuid )
        {
            boost::shared_ptr<T> obj = ::boost::static_pointer_cast<T>( i->first.lock() );
            OSLM_TRACE( "UUID::get with uuid=" << uuid << " submarine found");
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


} // end namespace fwTools

