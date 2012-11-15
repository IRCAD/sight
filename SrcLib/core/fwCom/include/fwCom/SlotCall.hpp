/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCALL_HPP__
#define __FWCOM_SLOTCALL_HPP__

#include <fwThread/Worker.hpp>

#include "fwCom/SlotRun.hpp"

namespace fwCom
{

template< typename F >
struct SlotCall;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3 >
struct SlotCall< R ( A1, A2, A3 ) > : SlotRun< void ( A1, A2, A3 ) >
{
    typedef R SignatureType( A1, A2, A3 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef ::boost::shared_future< R > SharedFutureType;

    SlotCall() :  SlotRun< void ( A1, A2, A3 ) >() {}

    virtual R call( A1, A2, A3 ) const = 0;

    virtual SharedFutureType asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3 ) const;

    virtual SharedFutureType asyncCall( A1 args1, A2 args2, A3 args3 ) const;

protected:
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3 ) const;
};
template< typename R, typename A1, typename A2 >
struct SlotCall< R ( A1, A2 ) > : SlotRun< void ( A1, A2 ) >
{
    typedef R SignatureType( A1, A2 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef ::boost::shared_future< R > SharedFutureType;

    SlotCall() :  SlotRun< void ( A1, A2 ) >() {}

    virtual R call( A1, A2 ) const = 0;

    virtual SharedFutureType asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2 ) const;

    virtual SharedFutureType asyncCall( A1 args1, A2 args2 ) const;

protected:
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2 ) const;
};
template< typename R, typename A1 >
struct SlotCall< R ( A1 ) > : SlotRun< void ( A1 ) >
{
    typedef R SignatureType( A1 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef ::boost::shared_future< R > SharedFutureType;

    SlotCall() :  SlotRun< void ( A1 ) >() {}

    virtual R call( A1 ) const = 0;

    virtual SharedFutureType asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1 ) const;

    virtual SharedFutureType asyncCall( A1 args1 ) const;

protected:
    virtual ::boost::function< R() > bindCall( A1 args1 ) const;
};
template< typename R>
struct SlotCall< R () > : SlotRun< void () >
{
    typedef R SignatureType();
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef ::boost::shared_future< R > SharedFutureType;

    SlotCall() :  SlotRun< void () >() {}

    virtual R call() const = 0;

    virtual SharedFutureType asyncCall( const ::fwThread::Worker::sptr &worker) const;

    virtual SharedFutureType asyncCall() const;

protected:
    virtual ::boost::function< R() > bindCall() const;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ...A >
struct SlotCall< R (A...) > : SlotRun< void ( A... ) >
{
    typedef R SignatureType(A...);
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;

    typedef ::boost::shared_future< R > SharedFutureType;

    SlotCall() :  SlotRun< void ( A... ) >() {}

    virtual R call(A...) const = 0;

    virtual SharedFutureType asyncCall( const ::fwThread::Worker::sptr &worker, A... args ) const;

    virtual SharedFutureType asyncCall( A... args ) const;

protected:
    virtual ::boost::function< R() > bindCall( A...args  ) const;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SLOTCALL_HPP__ */

