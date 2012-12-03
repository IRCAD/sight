/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTRUN_HPP__
#define __FWCOM_SLOTRUN_HPP__

#include <set>

#include <boost/function.hpp>

#include "fwCom/SlotBase.hpp"

fwCorePredeclare( (fwThread)(Worker) );

namespace fwCom
{

template< typename F >
struct SlotRun;

/**
 * @brief Slot run interface (call without return value).
 *
 * @tparam A Slot's arguments types.
 */
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
struct SlotRun< void ( A1, A2, A3 ) > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType( A1, A2, A3 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    /**  @} */

    SlotRun() : SlotBase( 3 ) {}

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run( A1  a1, A2  a2, A3  a3 ) const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2, A3 args3 ) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1, A2 args2, A3 args3 ) const;

protected:

    template< typename R, typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the run method within a void() function.
     *
     * @return a void() function.
     */
    virtual ::boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3 ) const;

};
template< typename A1, typename A2 >
struct SlotRun< void ( A1, A2 ) > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType( A1, A2 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    /**  @} */

    SlotRun() : SlotBase( 2 ) {}

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run( A1  a1, A2  a2 ) const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker) &worker, A1 args1, A2 args2 ) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1, A2 args2 ) const;

protected:

    template< typename R, typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the run method within a void() function.
     *
     * @return a void() function.
     */
    virtual ::boost::function< void() > bindRun( A1 args1, A2 args2 ) const;

};
template< typename A1 >
struct SlotRun< void ( A1 ) > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType( A1 );
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    /**  @} */

    SlotRun() : SlotBase( 1 ) {}

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run( A1  a1 ) const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker) &worker, A1 args1 ) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( A1 args1 ) const;

protected:

    template< typename R, typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the run method within a void() function.
     *
     * @return a void() function.
     */
    virtual ::boost::function< void() > bindRun( A1 args1 ) const;

};
template<>
struct SlotRun< void () > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType();
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    /**  @} */

    SlotRun() : SlotBase( 0 ) {}

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run() const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker) &worker) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun() const;

protected:

    template< typename R, typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the run method within a void() function.
     *
     * @return a void() function.
     */
    virtual ::boost::function< void() > bindRun() const;

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
struct SlotRun< void (A...) > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType(A...);
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;
    /**  @} */

    SlotRun() : SlotBase( sizeof...(A) ) {}

    /**
     * @brief Run the Slot with the given parameters.
     */
    virtual void run(A ... a) const = 0;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     *
     * @param worker Worker that will run the Slot.
     * @param args run arguments.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if given worker is not valid.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker) &worker, A... args ) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun(A... args) const;

protected:

    template< typename R, typename WEAKCALL >
    static ::boost::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker) &worker, WEAKCALL f );

    /**
     * @brief Binds the given parameters to the run method within a void() function.
     *
     * @return a void() function.
     */
    virtual ::boost::function< void() > bindRun( A...args  ) const;

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES



} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HPP__ */


