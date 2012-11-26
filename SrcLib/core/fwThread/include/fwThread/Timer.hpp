/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_TIMER_HPP__
#define __FWTHREAD_TIMER_HPP__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <fwCore/mt/types.hpp>

#include "fwThread/config.hpp"
#include "fwThread/Worker.hpp"


namespace fwThread
{

/**
 * @class Timer
 * @brief The Timer class provides single-shot or repetitive timers.
 * A Timer triggers a function once after a delay, or periodically, inside the
 * worker loop. The delay or the period is defined by the duration attribute.
 *
 * Timer's function must be set before starting the timer.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWTHREAD_CLASS_API Timer : public ::fwCore::BaseObject
{
public:
    /**
     * @name Typedefs
     * @{ */
    typedef ::boost::function< void() >         FunctionType;
    typedef ::boost::posix_time::time_duration  TimeDurationType;
    /**  @} */


    fwCoreClassDefinitionsWithFactoryMacro( (Timer)(::fwCore::BaseObject),
                                            ( ((Worker::sptr)) ((bool)( false )) ),
                                            ::boost::make_shared< Timer > );

    /**
     * @brief Constructs a Timer from given worker.
     * By default, a the Timer mode is repetitive.
     */
    FWTHREAD_API Timer(Worker::sptr worker, bool oneShot = false);

    FWTHREAD_API ~Timer();

    /// Starts or restarts the timer.
    FWTHREAD_API void start();

    /// Stops the timer and cancel all pending operations.
    FWTHREAD_API void stop();

    /// Sets time duration.
    FWTHREAD_API void setDuration(TimeDurationType duration);

    /// Sets the function to be triggered when time duration expires.
    template< typename F >
    void setFunction(F f)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_function = f;
    }

    /// Returns if the timer mode is 'one shot'.
    bool isOneShot() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_oneShot;
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_oneShot = oneShot;
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_running;
    }

protected:

    FWTHREAD_API void cancelNoLock();
    FWTHREAD_API void rearmNoLock(TimeDurationType duration);

    FWTHREAD_API void call(const ::boost::system::error_code & code);


    /// Copy constructor forbidden.
    Timer( const Timer& );

    /// Copy operator forbidden.
    Timer& operator=( const Timer& );

    /// Timer object.
    ::boost::asio::deadline_timer m_timer;

    /// Function object to execute each time the timer expires.
    FunctionType      m_function;

    /// Time to wait until timer's expiration.
    TimeDurationType m_duration;

    /// Timer's worker.
    Worker::sptr     m_worker;

    /// Timer's mode.
    bool             m_oneShot;

    /// Timer's state.
    bool             m_running;

    mutable ::fwCore::mt::Mutex m_mutex;
};

} //namespace fwThread


#endif //__FWTHREAD_TIMER_HPP__
