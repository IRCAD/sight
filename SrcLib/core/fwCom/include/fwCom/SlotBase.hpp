/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTBASE_HPP__
#define __FWCOM_SLOTBASE_HPP__

#include <set>

#include <boost/thread/future.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwThread/Worker.hpp>

#include "fwCom/config.hpp"
#include "fwCom/util/convert_function_type.hpp"
#include "fwCom/SlotConnectionBase.hpp"

namespace fwCom
{

template< typename F >
struct SlotRun;

template< typename F >
struct Slot;

struct FWCOM_CLASS_API SlotBase : virtual fwCore::BaseObject
{

    typedef SPTR( SlotBase ) sptr;
    typedef WPTR( SlotBase ) wptr;
    typedef SPTR( SlotBase const ) csptr;
    typedef WPTR( SlotBase const ) cwptr;

    typedef ::boost::shared_future< void > VoidSharedFutureType;
    typedef std::set< SlotConnectionBase::csptr > ConnectionSetType;

    virtual ~SlotBase() {};

    unsigned int arity() const
    {
        return m_arity;
    }

    void setWorker(const ::fwThread::Worker::sptr &worker)
    {
        m_worker = worker;
    }

    ::fwThread::Worker::sptr getWorker() const
    {
        return m_worker;
    }

    template< typename A1, typename A2, typename A3 > void run(A1 a1, A2 a2, A3 a3) const;
    template< typename A1, typename A2 > void run(A1 a1, A2 a2) const;
    template< typename A1 > void run(A1 a1) const;
    FWCOM_API void run() const;

    template< typename R, typename A1, typename A2, typename A3 > R call(A1 a1, A2 a2, A3 a3) const;
    template< typename R, typename A1, typename A2 > R call(A1 a1, A2 a2) const;
    template< typename R, typename A1 > R call(A1 a1) const;
    template< typename R > R call() const;

    template< typename A1, typename A2, typename A3 > VoidSharedFutureType asyncRun(A1 a1, A2 a2, A3 a3) const;
    template< typename A1, typename A2 > VoidSharedFutureType asyncRun(A1 a1, A2 a2) const;
    template< typename A1 > VoidSharedFutureType asyncRun(A1 a1) const;
    FWCOM_API VoidSharedFutureType asyncRun() const;

    template< typename R, typename A1, typename A2, typename A3 > ::boost::shared_future< R > asyncCall(A1 a1, A2 a2, A3 a3) const;
    template< typename R, typename A1, typename A2 > ::boost::shared_future< R > asyncCall(A1 a1, A2 a2) const;
    template< typename R, typename A1 > ::boost::shared_future< R > asyncCall(A1 a1) const;
    template< typename R > ::boost::shared_future< R > asyncCall() const;


protected:

    template < typename F >
    friend struct SlotConnection;

    template < typename F >
    friend struct Signal;


    template < typename F >
    std::string getTypeName() const
    {
        std::string signature = std::string("function_type(") + typeid(F).name() + ")";
        return signature;
    }

    SlotBase(unsigned int arity) : m_arity(arity) {};




    std::string m_signature;

    const unsigned int m_arity;

    ::fwThread::Worker::sptr m_worker;

    ConnectionSetType m_connections;

};

} // namespace fwCom

#endif /* __FWCOM_SLOTBASE_HPP__ */

