/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCALL_HPP__
#define __FWCOM_SLOTCALL_HPP__

#include "fwCom/SlotRun.hpp"

fwCorePredeclare( (fwThread)(Worker) );

namespace fwCom
{

template< typename F >
struct SlotCall;

/**
 * @brief Slot call interface (call with return value).
 *
 * @tparam R Slot's return type.
 * @tparam A Slot's arguments types.
 */
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct SlotCall< R ( A1, A2, A3, A4, A5 ) > : SlotRun< void ( A1, A2, A3, A4, A5 ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2, A3, A4, A5 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A1, A2, A3, A4, A5 ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call( A1, A2, A3, A4, A5 ) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const;
};
template< typename R, typename A1, typename A2, typename A3, typename A4 >
struct SlotCall< R ( A1, A2, A3, A4 ) > : SlotRun< void ( A1, A2, A3, A4 ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2, A3, A4 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A1, A2, A3, A4 ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call( A1, A2, A3, A4 ) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2, A3 args3, A4 args4 ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A1 args1, A2 args2, A3 args3, A4 args4 ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4 ) const;
};
template< typename R, typename A1, typename A2, typename A3 >
struct SlotCall< R ( A1, A2, A3 ) > : SlotRun< void ( A1, A2, A3 ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2, A3 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A1, A2, A3 ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call( A1, A2, A3 ) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2, A3 args3 ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A1 args1, A2 args2, A3 args3 ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3 ) const;
};
template< typename R, typename A1, typename A2 >
struct SlotCall< R ( A1, A2 ) > : SlotRun< void ( A1, A2 ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1, A2 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A1, A2 ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call( A1, A2 ) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2 ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A1 args1, A2 args2 ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A1 args1, A2 args2 ) const;
};
template< typename R, typename A1 >
struct SlotCall< R ( A1 ) > : SlotRun< void ( A1 ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType( A1 );
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A1 ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call( A1 ) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A1 args1 ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A1 args1 ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A1 args1 ) const;
};
template< typename R>
struct SlotCall< R () > : SlotRun< void () >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType();
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void () >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call() const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall() const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall() const;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ...A >
struct SlotCall< R (A...) > : SlotRun< void ( A... ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType(A...);
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    typedef ::boost::shared_future< R > SharedFutureType;
    /**  @} */


    SlotCall() :  SlotRun< void ( A... ) >() {}

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call(A...) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker) &worker, A... args ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A... args ) const;

protected:

    template< typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the call method within a R() function.
     *
     * @return a R() function.
     */
    virtual ::boost::function< R() > bindCall( A...args  ) const;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SLOTCALL_HPP__ */

