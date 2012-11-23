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
 * @class   Timer
 * @brief  TODO
 * this thread.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWTHREAD_CLASS_API Timer : public ::fwCore::BaseObject
{
public:
    typedef ::boost::function< void() >         FunctorType;
    typedef ::boost::posix_time::time_duration  TimeDurationType;


    fwCoreClassDefinitionsWithFactoryMacro( (Timer)(::fwCore::BaseObject),
                                            ( ((Worker::sptr)) ((bool)( false )) ),
                                            ::boost::make_shared< Timer > );

    FWTHREAD_API Timer(Worker::sptr worker, bool oneShot = false);

    FWTHREAD_API ~Timer();

    FWTHREAD_API void start();
    FWTHREAD_API void stop();

    void setDuration(TimeDurationType duration);

    template< typename F >
    void setFunctor(F f)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_functor = f;
    }

    bool isOneShot() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_oneShot;
    }
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_oneShot = oneShot;
    }

    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_running;
    }

protected:

    FWTHREAD_API void cancelNoLock();
    FWTHREAD_API void rearmNoLock(TimeDurationType duration);

    FWTHREAD_API void call(const ::boost::system::error_code & code);


    /// Copy constructor forbidden
    Timer( const Timer& );

    /// Copy operator forbidden
    Timer& operator=( const Timer& );


    ::boost::asio::deadline_timer m_timer;
    FunctorType      m_functor;
    TimeDurationType m_duration;
    Worker::sptr     m_worker;
    bool             m_oneShot;
    bool             m_running;

    FWTHREAD_API mutable ::fwCore::mt::Mutex m_mutex;
};

} //namespace fwThread


#endif //__FWTHREAD_TIMER_HPP__
