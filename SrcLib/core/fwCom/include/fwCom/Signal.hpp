/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SIGNAL_HPP__
#define __FWCOM_SIGNAL_HPP__

#include <list>
#include <map>
#include <utility>

#include <boost/type_traits/is_same.hpp>

#include <fwCore/mt/types.hpp>

#include "fwCom/SignalBase.hpp"
#include "fwCom/SlotConnectionBase.hpp"
#include "fwCom/SlotRun.hpp"


namespace fwCom
{

template < typename F >
struct Signal;

/**
 * @brief Signal implementation.
 * Template parameter T must always be void.
 */
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
struct Signal< R ( A1, A2, A3 ) > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2, A3 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase::wptr, SlotConnectionBase::wptr > ConnectionMapType;
    /**  @} */

    /// Constructs a new Signal of type Signal<R( A1, A2, A3 )>.
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
    Connection connect ( SlotBase::sptr slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect ( SlotBase::sptr slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit( A1 a1, A2 a2, A3 a3 ) const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit( A1 a1, A2 a2, A3 a3 ) const;

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
    Connection getConnection( SlotBase::sptr slot, bool throws = false );

protected:

    template < typename F >
    friend struct SlotConnection;

    /**
     * @brief Connects the given slot.
     * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
     * returns a connection on succes, otherwise try to connect
     * the same slot with the type R (A1, A2, ..., Am).
     *
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    template< typename FROM_F >
    Connection connect( SlotBase::sptr slot );

    /// Connected slots.
    SlotContainerType m_slots;

    /// Container of current connections.
    ConnectionMapType m_connections;

    mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );

};
template < typename R, typename A1, typename A2 >
struct Signal< R ( A1, A2 ) > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase::wptr, SlotConnectionBase::wptr > ConnectionMapType;
    /**  @} */

    /// Constructs a new Signal of type Signal<R( A1, A2 )>.
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
    Connection connect ( SlotBase::sptr slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect ( SlotBase::sptr slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit( A1 a1, A2 a2 ) const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit( A1 a1, A2 a2 ) const;

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
    Connection getConnection( SlotBase::sptr slot, bool throws = false );

protected:

    template < typename F >
    friend struct SlotConnection;

    /**
     * @brief Connects the given slot.
     * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
     * returns a connection on succes, otherwise try to connect
     * the same slot with the type R (A1, A2, ..., Am).
     *
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    template< typename FROM_F >
    Connection connect( SlotBase::sptr slot );

    /// Connected slots.
    SlotContainerType m_slots;

    /// Container of current connections.
    ConnectionMapType m_connections;

    mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );

};
template < typename R, typename A1 >
struct Signal< R ( A1 ) > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase::wptr, SlotConnectionBase::wptr > ConnectionMapType;
    /**  @} */

    /// Constructs a new Signal of type Signal<R( A1 )>.
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
    Connection connect ( SlotBase::sptr slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect ( SlotBase::sptr slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit( A1 a1 ) const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit( A1 a1 ) const;

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
    Connection getConnection( SlotBase::sptr slot, bool throws = false );

protected:

    template < typename F >
    friend struct SlotConnection;

    /**
     * @brief Connects the given slot.
     * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
     * returns a connection on succes, otherwise try to connect
     * the same slot with the type R (A1, A2, ..., Am).
     *
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    template< typename FROM_F >
    Connection connect( SlotBase::sptr slot );

    /// Connected slots.
    SlotContainerType m_slots;

    /// Container of current connections.
    ConnectionMapType m_connections;

    mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );

};
template < typename R>
struct Signal< R () > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType();

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase::wptr, SlotConnectionBase::wptr > ConnectionMapType;
    /**  @} */

    /// Constructs a new Signal of type Signal<R()>.
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
    Connection connect ( SlotBase::sptr slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect ( SlotBase::sptr slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit() const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit() const;

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
    Connection getConnection( SlotBase::sptr slot, bool throws = false );

protected:

    template < typename F >
    friend struct SlotConnection;

    /**
     * @brief Connects the given slot.
     * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
     * returns a connection on succes, otherwise try to connect
     * the same slot with the type R (A1, A2, ..., Am).
     *
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    template< typename FROM_F >
    Connection connect( SlotBase::sptr slot );

    /// Connected slots.
    SlotContainerType m_slots;

    /// Container of current connections.
    ConnectionMapType m_connections;

    mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
struct Signal< R (A...) > : SignalBase
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType(A...);

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase::wptr, SlotConnectionBase::wptr > ConnectionMapType;
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
    Connection connect ( SlotBase::sptr slot );

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect ( SlotBase::sptr slot );

    /// Disconnects all slots.
    void disconnectAll();

    /// Requests execution of slots with given arguments.
    void emit( A...a ) const;

    /// Requests asynchronous execution of slots with given arguments.
    void asyncEmit( A...a ) const;

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
    Connection getConnection( SlotBase::sptr slot, bool throws = false );

protected:

    template < typename F >
    friend struct SlotConnection;

    /**
     * @brief Connects the given slot.
     * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
     * returns a connection on succes, otherwise try to connect
     * the same slot with the type R (A1, A2, ..., Am).
     *
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    template< typename FROM_F >
    Connection connect( SlotBase::sptr slot );

    /// Connected slots.
    SlotContainerType m_slots;

    /// Container of current connections.
    ConnectionMapType m_connections;

    mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SIGNAL_HPP__ */

