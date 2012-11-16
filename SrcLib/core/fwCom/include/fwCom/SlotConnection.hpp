/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCONNECTION_HPP__
#define __FWCOM_SLOTCONNECTION_HPP__

#include "fwCom/exception/BadSlot.hpp"
#include "fwCom/SlotBase.hpp"
#include "fwCom/SlotConnectionBase.hpp"


namespace fwCom
{

template < typename F >
struct Signal;

template < typename F >
struct SlotConnection;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
struct SlotConnection< void ( A1, A2, A3 ) > : SlotConnectionBase
{
    typedef void SignatureType( A1, A2, A3 );
    typedef SlotConnection< SignatureType > SelfType;


    typedef SPTR( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR( SignalType )      SignalSptrType;
    typedef WPTR( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;

    static sptr New( const SignalSptrType &signal, const SlotRunSptrType &slot );

    static sptr New( const SignalSptrType &signal,
                     const SlotBase::sptr &slot,
                     const SlotWrapperSptrType &slotWrapper
                    );

    void disconnect();

    ~SlotConnection();


    SlotConnection( const SignalSptrType &signal, const SlotRunSptrType &slot );

    SlotConnection( const SignalSptrType &signal,
                    const SlotBase::sptr &slot,
                    const SlotWrapperSptrType &slotWrapper
                    );


protected:

    template < typename F >
    friend struct Signal;

    void connect();

    SlotConnectionBase::BlockerSptrType getBlocker();

    void unblock();



    SignalWptrType m_signal;
    SlotBase::wptr m_connectedSlot;
    SlotWrapperSptrType m_slotWrapper;

    PairType m_pair;
    SlotConnectionBase::BlockerWptrType m_weakBlocker;
};
template < typename A1, typename A2 >
struct SlotConnection< void ( A1, A2 ) > : SlotConnectionBase
{
    typedef void SignatureType( A1, A2 );
    typedef SlotConnection< SignatureType > SelfType;


    typedef SPTR( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR( SignalType )      SignalSptrType;
    typedef WPTR( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;

    static sptr New( const SignalSptrType &signal, const SlotRunSptrType &slot );

    static sptr New( const SignalSptrType &signal,
                     const SlotBase::sptr &slot,
                     const SlotWrapperSptrType &slotWrapper
                    );

    void disconnect();

    ~SlotConnection();


    SlotConnection( const SignalSptrType &signal, const SlotRunSptrType &slot );

    SlotConnection( const SignalSptrType &signal,
                    const SlotBase::sptr &slot,
                    const SlotWrapperSptrType &slotWrapper
                    );


protected:

    template < typename F >
    friend struct Signal;

    void connect();

    SlotConnectionBase::BlockerSptrType getBlocker();

    void unblock();



    SignalWptrType m_signal;
    SlotBase::wptr m_connectedSlot;
    SlotWrapperSptrType m_slotWrapper;

    PairType m_pair;
    SlotConnectionBase::BlockerWptrType m_weakBlocker;
};
template < typename A1 >
struct SlotConnection< void ( A1 ) > : SlotConnectionBase
{
    typedef void SignatureType( A1 );
    typedef SlotConnection< SignatureType > SelfType;


    typedef SPTR( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR( SignalType )      SignalSptrType;
    typedef WPTR( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;

    static sptr New( const SignalSptrType &signal, const SlotRunSptrType &slot );

    static sptr New( const SignalSptrType &signal,
                     const SlotBase::sptr &slot,
                     const SlotWrapperSptrType &slotWrapper
                    );

    void disconnect();

    ~SlotConnection();


    SlotConnection( const SignalSptrType &signal, const SlotRunSptrType &slot );

    SlotConnection( const SignalSptrType &signal,
                    const SlotBase::sptr &slot,
                    const SlotWrapperSptrType &slotWrapper
                    );


protected:

    template < typename F >
    friend struct Signal;

    void connect();

    SlotConnectionBase::BlockerSptrType getBlocker();

    void unblock();



    SignalWptrType m_signal;
    SlotBase::wptr m_connectedSlot;
    SlotWrapperSptrType m_slotWrapper;

    PairType m_pair;
    SlotConnectionBase::BlockerWptrType m_weakBlocker;
};
template <>
struct SlotConnection< void () > : SlotConnectionBase
{
    typedef void SignatureType();
    typedef SlotConnection< SignatureType > SelfType;


    typedef SPTR( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR( SignalType )      SignalSptrType;
    typedef WPTR( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;

    static sptr New( const SignalSptrType &signal, const SlotRunSptrType &slot );

    static sptr New( const SignalSptrType &signal,
                     const SlotBase::sptr &slot,
                     const SlotWrapperSptrType &slotWrapper
                    );

    void disconnect();

    ~SlotConnection();


    SlotConnection( const SignalSptrType &signal, const SlotRunSptrType &slot );

    SlotConnection( const SignalSptrType &signal,
                    const SlotBase::sptr &slot,
                    const SlotWrapperSptrType &slotWrapper
                    );


protected:

    template < typename F >
    friend struct Signal;

    void connect();

    SlotConnectionBase::BlockerSptrType getBlocker();

    void unblock();



    SignalWptrType m_signal;
    SlotBase::wptr m_connectedSlot;
    SlotWrapperSptrType m_slotWrapper;

    PairType m_pair;
    SlotConnectionBase::BlockerWptrType m_weakBlocker;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
struct SlotConnection< void (A...) > : SlotConnectionBase
{
    typedef void SignatureType( A... );
    typedef SlotConnection< SignatureType > SelfType;


    typedef SPTR( SelfType ) sptr;

    typedef Signal< SignatureType > SignalType;
    typedef SPTR( SignalType )      SignalSptrType;
    typedef WPTR( SignalType )      SignalWptrType;

    typedef SlotRun< SignatureType > SlotWrapperType;
    typedef SPTR( SlotWrapperType )  SlotWrapperSptrType;

    typedef SlotRun< SignatureType > SlotRunType;
    typedef SPTR( SlotRunType )      SlotRunSptrType;

    typedef std::pair< bool, SlotRunType* > PairType;

    static sptr New( const SignalSptrType &signal, const SlotRunSptrType &slot );

    static sptr New( const SignalSptrType &signal,
                     const SlotBase::sptr &slot,
                     const SlotWrapperSptrType &slotWrapper
                    );

    void disconnect();

    ~SlotConnection();


    SlotConnection( const SignalSptrType &signal, const SlotRunSptrType &slot );

    SlotConnection( const SignalSptrType &signal,
                    const SlotBase::sptr &slot,
                    const SlotWrapperSptrType &slotWrapper
                    );


protected:

    template < typename F >
    friend struct Signal;

    void connect();

    SlotConnectionBase::BlockerSptrType getBlocker();

    void unblock();



    SignalWptrType m_signal;
    SlotBase::wptr m_connectedSlot;
    SlotWrapperSptrType m_slotWrapper;

    PairType m_pair;
    SlotConnectionBase::BlockerWptrType m_weakBlocker;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTION_HPP__ */

