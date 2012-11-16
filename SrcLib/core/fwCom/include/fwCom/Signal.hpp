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

#include "fwCom/SignalBase.hpp"
#include "fwCom/SlotConnectionBase.hpp"
#include "fwCom/SlotRun.hpp"


namespace fwCom
{

template < typename F >
struct Signal;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
struct Signal< R ( A1, A2, A3 ) > : SignalBase
{

    typedef R SignatureType( A1, A2, A3 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase*, SlotConnectionBase::wptr > ConnectionMapType;

    static sptr New();

    ~Signal()
    {
        this->disconnectAll();
    }

    Connection connect ( SlotBase::sptr slot );

    void disconnect ( SlotBase::sptr slot );

    void disconnectAll();

    void emit( A1 a1, A2 a2, A3 a3 ) const;

    void asyncEmit( A1 a1, A2 a2, A3 a3 ) const;

    size_t getNumberOfConnections() { return m_slots.size(); }

protected:

    template < typename F >
    friend struct SlotConnection;

    template< typename FROM_F >
    Connection connect ( SlotBase::sptr slot );

    SlotContainerType m_slots;
    ConnectionMapType m_connections;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );

};
template < typename R, typename A1, typename A2 >
struct Signal< R ( A1, A2 ) > : SignalBase
{

    typedef R SignatureType( A1, A2 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase*, SlotConnectionBase::wptr > ConnectionMapType;

    static sptr New();

    ~Signal()
    {
        this->disconnectAll();
    }

    Connection connect ( SlotBase::sptr slot );

    void disconnect ( SlotBase::sptr slot );

    void disconnectAll();

    void emit( A1 a1, A2 a2 ) const;

    void asyncEmit( A1 a1, A2 a2 ) const;

    size_t getNumberOfConnections() { return m_slots.size(); }

protected:

    template < typename F >
    friend struct SlotConnection;

    template< typename FROM_F >
    Connection connect ( SlotBase::sptr slot );

    SlotContainerType m_slots;
    ConnectionMapType m_connections;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );

};
template < typename R, typename A1 >
struct Signal< R ( A1 ) > : SignalBase
{

    typedef R SignatureType( A1 );

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase*, SlotConnectionBase::wptr > ConnectionMapType;

    static sptr New();

    ~Signal()
    {
        this->disconnectAll();
    }

    Connection connect ( SlotBase::sptr slot );

    void disconnect ( SlotBase::sptr slot );

    void disconnectAll();

    void emit( A1 a1 ) const;

    void asyncEmit( A1 a1 ) const;

    size_t getNumberOfConnections() { return m_slots.size(); }

protected:

    template < typename F >
    friend struct SlotConnection;

    template< typename FROM_F >
    Connection connect ( SlotBase::sptr slot );

    SlotContainerType m_slots;
    ConnectionMapType m_connections;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );

};
template < typename R>
struct Signal< R () > : SignalBase
{

    typedef R SignatureType();

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase*, SlotConnectionBase::wptr > ConnectionMapType;

    static sptr New();

    ~Signal()
    {
        this->disconnectAll();
    }

    Connection connect ( SlotBase::sptr slot );

    void disconnect ( SlotBase::sptr slot );

    void disconnectAll();

    void emit() const;

    void asyncEmit() const;

    size_t getNumberOfConnections() { return m_slots.size(); }

protected:

    template < typename F >
    friend struct SlotConnection;

    template< typename FROM_F >
    Connection connect ( SlotBase::sptr slot );

    SlotContainerType m_slots;
    ConnectionMapType m_connections;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
struct Signal< R (A...) > : SignalBase
{

    typedef R SignatureType(A...);

    typedef Signal< SignatureType > SelfType;

    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotSptr;

    typedef std::pair< bool, SlotRunType* > PairType;
    typedef std::list< PairType* > SlotContainerType;

    typedef std::map< SlotBase*, SlotConnectionBase::wptr > ConnectionMapType;

    static sptr New();

    ~Signal()
    {
        this->disconnectAll();
    }

    Connection connect ( SlotBase::sptr slot );

    void disconnect ( SlotBase::sptr slot );

    void disconnectAll();

    void emit( A...a ) const;

    void asyncEmit( A...a ) const;

    size_t getNumberOfConnections() { return m_slots.size(); }

protected:

    template < typename F >
    friend struct SlotConnection;

    template< typename FROM_F >
    Connection connect ( SlotBase::sptr slot );

    SlotContainerType m_slots;
    ConnectionMapType m_connections;

private:
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SIGNAL_HPP__ */

