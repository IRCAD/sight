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

#ifndef __FWCOM_SLOTBASE_HPP__
#define __FWCOM_SLOTBASE_HPP__

#include "fwCom/config.hpp"
#include "fwCom/util/convert_function_type.hpp"

#include <fwCore/BaseObject.hpp>
#include <fwCore/mt/types.hpp>
#include <fwCore/spyLog.hpp>

#include <future>
#include <queue>
#include <set>

namespace fwThread
{
class Worker;
}

namespace fwCom
{

namespace util
{
template< typename T, typename R >
struct WeakCall;
}

template< typename F >
struct SlotRun;

template< typename F >
class Slot;

struct SlotConnectionBase;

/**
 * @brief Base class for Slot implementations.
 */
struct FWCOM_CLASS_API SlotBase : virtual fwCore::BaseObject
{

    /**
     * @name typedefs
     * @{
     * Slot pointer types. */
    typedef SPTR ( SlotBase ) sptr;
    typedef WPTR ( SlotBase ) wptr;
    typedef SPTR ( SlotBase const ) csptr;
    typedef WPTR ( SlotBase const ) cwptr;

    typedef std::string IDType;
    /**  @} */

    /// SlotBase::asyncRun return type.
    typedef std::shared_future< void > VoidSharedFutureType;

    /// Connections container type
    typedef std::set< CSPTR( SlotConnectionBase ) > ConnectionSetType;

    virtual ~SlotBase()
    {
    }

    /**
     * @brief Returns Slot's arity.
     * The arity defines the number of parameters in Slot signature.
     */
    unsigned int arity() const
    {
        return m_arity;
    }

    /// Sets Slot's Worker.
    void setWorker(const SPTR(::fwThread::Worker)& worker)
    {
        ::fwCore::mt::WriteLock lock(m_workerMutex);

        m_worker = worker;
    }

    /// Returns Slot's Worker.
    SPTR(::fwThread::Worker) getWorker() const
    {
        ::fwCore::mt::ReadLock lock(m_workerMutex);
        return m_worker;
    }

    /**
     * @brief  Run the Slot.
     * @throw  BadRun if given arguments do not match the slot implementation
     */
    /**
     * @name Run helpers
     * @{ */
    template< typename A1, typename A2, typename A3 >
    void run(A1 a1, A2 a2, A3 a3) const;

    template< typename A1, typename A2 >
    void run(A1 a1, A2 a2) const;

    template< typename A1 >
    void run(A1 a1) const;

    FWCOM_API void run() const;
    /**  @} */

    /**
     * @brief  Call the Slot (with return value).
     * @throw  BadCall if given arguments do not match the slot implementation
     */
    /**
     * @name Call helpers
     * @{ */
    template< typename R, typename A1, typename A2, typename A3 >
    R call(A1 a1, A2 a2, A3 a3) const;

    template< typename R, typename A1, typename A2 >
    R call(A1 a1, A2 a2) const;

    template< typename R, typename A1 >
    R call(A1 a1) const;

    template< typename R >
    R call() const;
    /**  @} */

    /**
     * @brief  Run the Slot asynchronously.
     * @throw  BadRun if given arguments do not match the slot implementation
     * @throws NoWorker if given worker is not valid.
     */
    /**
     * @name Asynchronous run helpers
     * @{ */
    template< typename A1, typename A2, typename A3 >
    VoidSharedFutureType asyncRun(A1 a1, A2 a2, A3 a3) const;

    template< typename A1, typename A2 >
    VoidSharedFutureType asyncRun(A1 a1, A2 a2) const;

    template< typename A1 >
    VoidSharedFutureType asyncRun(A1 a1) const;

    FWCOM_API VoidSharedFutureType asyncRun() const;
    /**  @} */

    /**
     * @brief  Call the Slot asynchronously (with return value).
     * @throw  BadCall if given arguments do not match the slot implementation
     * @throws NoWorker if given worker is not valid.
     */
    /**
     * @name Asynchronous call helpers
     * @{ */
    template< typename R, typename A1, typename A2, typename A3 >
    std::shared_future< R > asyncCall(A1 a1, A2 a2, A3 a3) const;

    template< typename R, typename A1, typename A2 >
    std::shared_future< R > asyncCall(A1 a1, A2 a2) const;

    template< typename R, typename A1 >
    std::shared_future< R > asyncCall(A1 a1) const;

    template< typename R >
    std::shared_future< R > asyncCall() const;
    /**  @} */

    /// Returns number of connections.
    size_t getNumberOfConnections() const
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);
        return m_connections.size();
    }
    protected:

        /// Copy constructor forbidden
        SlotBase( const SlotBase& );

        /// Copy operator forbidden
        SlotBase& operator=( const SlotBase& );

        /**
         * @name SlotBase's friends
         * @{ */
        template < typename F >
        friend struct SlotConnection;

        template < typename F >
        friend struct Signal;

        template< typename T, typename R >
        friend struct util::WeakCall;

        /**  @} */

        /// Returns F typeid name.
        template < typename F >
        std::string getTypeName() const
        {
            std::string signature = std::string("function_type(") + typeid(F).name() + ")";
            return signature;
        }

        SlotBase(unsigned int arity) :
            m_arity(arity)
        {
        }

        /// Slot's signature based on typeid.
        std::string m_signature;

        /// Slot's arity.
        const unsigned int m_arity;

        /// Slot's Worker.
        SPTR(::fwThread::Worker) m_worker;

        /// Container of current connections.
        ConnectionSetType m_connections;

        mutable ::fwCore::mt::ReadWriteMutex m_connectionsMutex;
        mutable ::fwCore::mt::ReadWriteMutex m_workerMutex;
};

} // namespace fwCom

#endif /* __FWCOM_SLOTBASE_HPP__ */

