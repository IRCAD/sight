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

#ifndef __FWCOM_SLOTCALL_HPP__
#define __FWCOM_SLOTCALL_HPP__

#include "fwCom/SlotRun.hpp"

namespace fwThread
{
class Worker;
}

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
template< typename R, typename ... A >
struct SlotCall< R(A ...) > : SlotRun< void ( A ... ) >
{
    /**
     * @name Typedefs
     * @{ */
    typedef R SignatureType (A ...);
    typedef SlotCall< SignatureType > SelfType;
    typedef SPTR ( SelfType ) sptr;
    typedef WPTR ( SelfType ) wptr;
    typedef std::shared_future< R > SharedFutureType;
    /**  @} */

    SlotCall() :
        SlotRun< void ( A ... ) >()
    {
    }

    virtual ~SlotCall()
    {
    }

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call(A ...) const = 0;

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
    virtual SharedFutureType asyncCall( const SPTR(::fwThread::Worker)& worker, A ... args ) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual SharedFutureType asyncCall( A ... args ) const;

    protected:

        template< typename WEAKCALL >
        static std::shared_future< R > postWeakCall( const SPTR(::fwThread::Worker)& worker, WEAKCALL f );

        /**
         * @brief Binds the given parameters to the call method within a R() function.
         *
         * @return a R() function.
         */
        virtual std::function< R() > bindCall( A ... args  ) const;
};

} // namespace fwCom

#endif /* __FWCOM_SLOTCALL_HPP__ */

