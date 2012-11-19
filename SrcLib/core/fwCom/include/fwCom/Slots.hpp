/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOTS_HPP__
#define __FWCOM_SLOTS_HPP__

#include "fwCom/Slot.hpp"
#include "fwCom/config.hpp"

namespace fwCom
{

/**
 * @class   Slots.
 * @brief   This class proposes a mapping between a SlotKeyType and a SlotBase.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWCOM_CLASS_API Slots
{
public:

    typedef std::string SlotKeyType;
    typedef std::map< SlotKeyType, SlotBase::sptr > SlotMapType;
    typedef std::vector < SlotKeyType > SlotKeyContainerType;

    /// Constructor, does nothing
    FWCOM_API Slots();

    /// Constructor, check if all slots are disconnected
    FWCOM_API virtual ~Slots();

    /// Registers SlotBase in m_slots
    FWCOM_API Slots& operator()( const SlotKeyType &key, const SlotBase::sptr &slot );

    /// Registers Slot  in m_slots
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
    Slots& operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2, A3 ) >) slot ); //{}


template<typename R, typename A1, typename A2 >
    Slots& operator()( const SlotKeyType &key, SPTR(Slot< R ( A1, A2 ) >) slot ); //{}


template<typename R, typename A1 >
    Slots& operator()( const SlotKeyType &key, SPTR(Slot< R ( A1 ) >) slot ); //{}


template<typename R>
    Slots& operator()( const SlotKeyType &key, SPTR(Slot< R () >) slot ); //{}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A>
    Slots& operator()( const SlotKeyType &key, SPTR(Slot< R (A...) >) slot ); //{}


#endif  // BOOST_NO_VARIADIC_TEMPLATES
/// Creates in intern a new slot from function and registers it in m_slots
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename F, typename A1, typename A2, typename A3 >
    Slots& operator()( const SlotKeyType &key, F f, A1 a1, A2 a2, A3 a3 ); //{}


template<typename F, typename A1, typename A2 >
    Slots& operator()( const SlotKeyType &key, F f, A1 a1, A2 a2 ); //{}


template<typename F, typename A1 >
    Slots& operator()( const SlotKeyType &key, F f, A1 a1 ); //{}


template<typename F>
    Slots& operator()( const SlotKeyType &key, F f); //{}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename ...A>
    Slots& operator()( const SlotKeyType &key, F f, A...a ); //{}


#endif  // BOOST_NO_VARIADIC_TEMPLATES
/// Returns the SlotBase associated to the key, if key does not exist, the ptr is null
    FWCOM_API SlotBase::sptr operator[]( const SlotKeyType &key ) const;

    /// Assigns the worker to all slots stored in m_slots
    FWCOM_API void setWorker( const ::fwThread::Worker::sptr &worker );

    /// Returns all SlotKeyType registered in m_slots
    FWCOM_API SlotKeyContainerType getSlotKeys() const;

protected:

    /// Copy constructor forbidden
    Slots( const Slots& );

    /// Copy constructor forbidden
    Slots& operator=( const Slots& );

    /// Association < key , SlotBase::sptr >
    SlotMapType m_slots;
};

} // namespace fwCom

#endif //__FWCOM_SLOTS_HPP__
