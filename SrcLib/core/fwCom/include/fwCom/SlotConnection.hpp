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

#pragma once

#define __FWCOM_SLOTCONNECTION_HPP__

#include "fwCom/SlotBase.hpp"
#include "fwCom/SlotConnectionBase.hpp"

namespace fwCom
{

template < typename F >
struct Signal;

template < typename F >
struct SlotConnection;

/**
 * @brief Slot connection implementation.
 * This class is for internal use purpose.
 */
template < typename ... A >
struct FWCOM_CLASS_API SlotConnection< void (A ...) > : SlotConnectionBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType ( A ... );
    typedef SlotConnection< SignatureType > SelfType;

    typedef SPTR ( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR ( SignalType )      SignalSptrType;
    typedef WPTR ( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR ( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR ( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;
    /**  @} */

    /// Build a new connection with the given signal and slot.
    static sptr New( const SignalSptrType& signal, const SlotRunSptrType& slot );

    /// Build a new connection with the given signal, slot and wrapper.
    static sptr New( const SignalSptrType& signal,
                     const SPTR( SlotBase )& slot,
                     const SlotWrapperSptrType& slotWrapper
                     );

    /// Disconnect the related slot.
    void disconnect();

    ~SlotConnection();

    /// Build a new connection with the given signal and slot.
    SlotConnection( const SignalSptrType& signal, const SlotRunSptrType& slot );

    /// Build a new connection with the given signal, slot and wrapper.
    SlotConnection( const SignalSptrType& signal,
                    const SPTR( SlotBase )& slot,
                    const SlotWrapperSptrType& slotWrapper
                    );

    protected:

        template < typename F >
        friend struct Signal;

        /// Connect the related Signal and Slot together.
        void connectNoLock();

        /// *NOT THREAD SAFE* Disconnect the related signal.
        void disconnectSignalNoLock(const SignalSptrType& sig);

        /// *NOT THREAD SAFE* Disconnect the related slot.
        void disconnectSlotNoLock(const SPTR( SlotBase )& slot);

        /// *NOT THREAD SAFE* Disconnect the related slot and signal.
        void disconnectWeakLock();

        /**
         * @brief Returns a blocker on the connection.
         * The connection is blocked until the returned shared pointer dies.
         */
        SlotConnectionBase::BlockerSptrType getBlocker();

        /// Unblock this connection.
        void unblock();

        /// Related Signal.
        SignalWptrType m_signal;

        /// Related Slot.
        WPTR( SlotBase ) m_connectedSlot;

        /// Slot wrapper.
        SlotWrapperSptrType m_slotWrapper;

        /**
         * @brief Associates state of this connection to related Slot.
         * If m_pair.first is false, the connection is blocked.
         */
        PairType m_pair;

        /// Connection blocker.
        SlotConnectionBase::BlockerWptrType m_weakBlocker;

        mutable ::fwCore::mt::ReadWriteMutex m_mutex;
};

} // namespace fwCom
