/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOTS_HXX__
#define __FWCOM_SLOTS_HXX__


#ifndef __FWCOM_SLOTS_HPP__
#error fwCom/Slots.hpp not included
#endif

#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"

//-----------------------------------------------------------------------------

namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2, A3, A4, A5 ) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


template<typename R, typename A1, typename A2, typename A3, typename A4 >
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2, A3, A4 ) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


template<typename R, typename A1, typename A2, typename A3 >
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2, A3 ) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


template<typename R, typename A1, typename A2 >
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2 ) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


template<typename R, typename A1 >
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R ( A1 ) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


template<typename R>
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R () >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A>
Slots& Slots::operator()( const SlotKeyType &key, SPTR(Slot< R (A...) >) slot )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( slot );
    m_slots.insert( SlotMapType::value_type(key, slotBase) );
    return *this;
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename F, typename A1, typename A2, typename A3, typename A4, typename A5 >
Slots& Slots::operator()( const SlotKeyType &key, F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a1, a2, a3, a4, a5 ) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



template<typename F, typename A1, typename A2, typename A3, typename A4 >
Slots& Slots::operator()( const SlotKeyType &key, F f, A1 a1, A2 a2, A3 a3, A4 a4 )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a1, a2, a3, a4 ) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



template<typename F, typename A1, typename A2, typename A3 >
Slots& Slots::operator()( const SlotKeyType &key, F f, A1 a1, A2 a2, A3 a3 )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a1, a2, a3 ) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



template<typename F, typename A1, typename A2 >
Slots& Slots::operator()( const SlotKeyType &key, F f, A1 a1, A2 a2 )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a1, a2 ) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



template<typename F, typename A1 >
Slots& Slots::operator()( const SlotKeyType &key, F f, A1 a1 )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a1 ) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



template<typename F>
Slots& Slots::operator()( const SlotKeyType &key, F f)
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename ...A>
Slots& Slots::operator()( const SlotKeyType &key, F f, A...a )
{
    SPTR( SlotBase ) slot = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a...) );
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif //__FWCOM_SLOTS_HXX__
