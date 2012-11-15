/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTRUN_HPP__
#define __FWCOM_SLOTRUN_HPP__

#include <set>

#include <boost/function.hpp>

#include <fwThread/Worker.hpp>

#include "fwCom/SlotBase.hpp"

namespace fwCom
{

template< typename F >
struct SlotRun;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
struct SlotRun< void ( A1, A2, A3 ) > : SlotBase
{

    typedef void SignatureType( A1, A2, A3 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    SlotRun() : SlotBase( 3 ) {}

    virtual void run( A1  a1, A2  a2, A3  a3 ) const = 0;

    virtual SlotBase::VoidSharedFutureType asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3 ) const;

    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1, A2 args2, A3 args3 ) const;

protected:
    virtual ::boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3 ) const;

};
template< typename A1, typename A2 >
struct SlotRun< void ( A1, A2 ) > : SlotBase
{

    typedef void SignatureType( A1, A2 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    SlotRun() : SlotBase( 2 ) {}

    virtual void run( A1  a1, A2  a2 ) const = 0;

    virtual SlotBase::VoidSharedFutureType asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2 ) const;

    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1, A2 args2 ) const;

protected:
    virtual ::boost::function< void() > bindRun( A1 args1, A2 args2 ) const;

};
template< typename A1 >
struct SlotRun< void ( A1 ) > : SlotBase
{

    typedef void SignatureType( A1 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    SlotRun() : SlotBase( 1 ) {}

    virtual void run( A1  a1 ) const = 0;

    virtual SlotBase::VoidSharedFutureType asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1 ) const;

    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1 ) const;

protected:
    virtual ::boost::function< void() > bindRun( A1 args1 ) const;

};
template<>
struct SlotRun< void () > : SlotBase
{

    typedef void SignatureType();
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    SlotRun() : SlotBase(0) {}

    virtual void run() const = 0;

    virtual SlotBase::VoidSharedFutureType asyncRun( const ::fwThread::Worker::sptr &worker) const;

    virtual SlotBase::VoidSharedFutureType asyncRun() const;

protected:
    virtual ::boost::function< void() > bindRun() const;

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
struct SlotRun< void (A...) > : SlotBase
{

    typedef void SignatureType(A...);
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    SlotRun() : SlotBase( sizeof...(A) ) {}

    virtual void run(A ... a) const = 0;

    virtual SlotBase::VoidSharedFutureType asyncRun( const ::fwThread::Worker::sptr &worker, A... args ) const;

    virtual SlotBase::VoidSharedFutureType asyncRun(A... args) const;

protected:
    virtual ::boost::function< void() > bindRun( A...args  ) const;

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HPP__ */


