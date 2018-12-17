/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWCOM_SIGNAL_HPP__
#define __FWCOM_SIGNAL_HPP__

#include "fwCom/SignalBase.hpp"
#include <fwCom/SlotConnection.hpp>

#include <fwCore/mt/types.hpp>

#include <list>
#include <map>
#include <type_traits>
#include <utility>

namespace fwCom
{

struct SlotConnectionBase;
struct SlotBase;

template < typename F >
struct Signal;

template < typename F >
struct SlotRun;

/**
 * @brief Signal implementation.
 * Template parameter T must always be void.
 */
template < typename R, typename ... A >
struct Signal< R(A ...) > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType (A ...);

    typedef Signal< SignatureType > SelfType;

    typedef SPTR ( SelfType ) sptr;
    typedef WPTR ( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR ( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< WPTR( SlotBase ), WPTR( SlotConnectionBase ),
                      std::owner_less< WPTR( SlotBase ) > > ConnectionMapType;
    /**  @} */

    /// Constructs a new Signal of type Signal<R(A...)>.
    static sptr New();

    /// Destructor : disconnects all remaining connections.
    ~Signal()
    {
        this->disconnectAll();
    }

    /**
     * @brief Connects the given slot.
     *
     * @return an object managing the connection.
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    Connection connect( SPTR( SlotBase ) slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect( SPTR( SlotBase ) slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit( A ... a ) const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit( A ... a ) const;

    /// Returns number of connected slots.
    size_t getNumberOfConnections() const
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);
        return m_slots.size();
    }

    /**
     * @brief Returns the connection handler matching given slot.
     * @throws BadSlot if given slot is not connected and `throws` is true.
     */
    Connection getConnection( SPTR( SlotBase ) slot, bool throws = false );

    protected:

        template < typename F >
        friend struct SlotConnection;
        /**
         * @brief Connects the given slot.
         * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
         * returns a connection on success, otherwise try to connect
         * the same slot with the type R (A1, A2, ..., Am).
         *
         * @throws BadSlot If given slot doesn't match signal type.
         * @throws AlreadyConnected If given slot is already connected.
         */
        template< typename FROM_F >
        Connection connect( SPTR( SlotBase ) slot );

        /// Connected slots.
        SlotContainerType m_slots;

        /// Container of current connections.
        ConnectionMapType m_connections;

        mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

    private:
        BOOST_STATIC_ASSERT( (std::is_same<void, R>::value) );

};

} // namespace fwCom

#endif /* __FWCOM_SIGNAL_HPP__ */

