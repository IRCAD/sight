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
#ifndef __FWCOM_SLOTRUN_HPP__
#define __FWCOM_SLOTRUN_HPP__

#include "fwCom/SlotBase.hpp"

#include <functional>
#include <set>

namespace fwThread
{
class Worker;
}

namespace fwCom
{

template< typename F >
struct SlotRun;

/**
 * @brief Slot run interface (call without return value).
 *
 * @tparam A Slot's arguments types.
 */
template< typename ... A >
struct SlotRun< void (A ...) > : SlotBase
{

    /**
     * @name Typedefs
     * @{ */
    typedef void SignatureType (A ...);
    typedef SlotRun< SignatureType > SelfType;
    typedef SPTR ( SelfType ) sptr;
    typedef WPTR ( SelfType ) wptr;
    /**  @} */

    SlotRun() :
        SlotBase( sizeof ... (A) )
    {
    }

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
    virtual SlotBase::VoidSharedFutureType asyncRun( const SPTR(::fwThread::Worker)& worker, A ... args ) const;

    /**
     * @brief Run the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's run result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SlotBase::VoidSharedFutureType asyncRun(A ... args) const;

    protected:

        template< typename R, typename WEAKCALL >
        static std::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker)& worker, WEAKCALL f );

        /**
         * @brief Binds the given parameters to the run method within a void() function.
         *
         * @return a void() function.
         */
        virtual std::function< void() > bindRun( A ... args  ) const;

};

} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HPP__ */

