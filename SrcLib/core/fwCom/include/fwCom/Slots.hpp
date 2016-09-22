/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOTS_HPP__
#define __FWCOM_SLOTS_HPP__

#include <map>
#include <vector>
#include <string>

#include "fwCore/macros.hpp"
#include "fwCom/config.hpp"

namespace fwThread
{
class Worker;
}

namespace fwCom
{

struct SlotBase;

template< typename F >
class Slot;

/**
 * @brief   This class proposes a mapping between a SlotKeyType and a SlotBase.
 */
class FWCOM_CLASS_API Slots
{
public:

    typedef std::string SlotKeyType;
    typedef std::map< SlotKeyType, SPTR( SlotBase ) > SlotMapType;
    typedef std::vector < SlotKeyType > SlotKeyContainerType;

    /// Constructor, does nothing
    FWCOM_API Slots();

    /// Constructor, check if all slots are disconnected
    FWCOM_API virtual ~Slots();

    /// Registers SlotBase in m_slots
    FWCOM_API Slots& operator()( const SlotKeyType& key, const SPTR( SlotBase )& slot );

    /// Registers Slot  in m_slots (defined here to avoid compiler error C2244)
    template<typename R, typename ... A>
    Slots& operator()( const SlotKeyType& key, SPTR(Slot< R (A ...) >)slot )
    {
        SPTR( SlotBase ) slotBase = std::dynamic_pointer_cast< SlotBase >( slot );
        return this->operator()(key, slotBase);
    }

    /// Creates in intern a new slot from function and registers it in m_slots
    template<typename F, typename ... A>
    Slots& operator()( const SlotKeyType& key, F f, A ... a );

    /// Returns the SlotBase associated to the key, if key does not exist, the ptr is null
    FWCOM_API SPTR( SlotBase ) operator[]( const SlotKeyType &key ) const;

    /// Assigns the worker to all slots stored in m_slots
    FWCOM_API void setWorker( const SPTR(::fwThread::Worker)& worker );

    /// Returns all SlotKeyType registered in m_slots
    FWCOM_API SlotKeyContainerType getSlotKeys() const;

protected:

    /// Copy constructor forbidden
    Slots( const Slots& );

    /// Copy constructor forbidden
    Slots& operator=( const Slots& );

    /// Association < key , SPTR( SlotBase ) >
    SlotMapType m_slots;
};

} // namespace fwCom

#endif //__FWCOM_SLOTS_HPP__
